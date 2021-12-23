该文档用于记录idea、实验结论、需要做的方向、算法细节等。细节要做的事情见notes.md。

## Ideas

- 时间主要花在计算任务 or 寻址？

# Definitions & Notations

## Task：

- 维护一个**key** $k \in \mathcal{K}$的集合$S$，支持插入/删除/查询。查询可以有false positive但是不能有false negative，即在集合内则必须回答在，不在集合内可以回答在。

- **Assumption 1：** 每次随机插入key。
  **Skew version**：插入具有大量重复的key。

- **Static version：**事先知道过程中最大的集合大小，filter可以提前预定好使用空间大小。

  **Dynamic version：** 事先不知道数据规模，过程中filter要保证使用空间与【开始到当前最大的集合大小】同阶。（大量插入+大量删除后，是否需要shrink？）

  - Dynamic version可以通过reconstruct static version得到。但是该方法使用空间不够平滑，且重构用时长。

## Evaluation

- **Throughput：效率指标** 
  插入tp，删除tp，查询tp（细分为查询positive tp，查询negative tp），重构tp。

  - 数据规模对throughput影响巨大，似乎是数据规模小的时候L3 Cache hit会比较多？
    （该结论由输入大小为$2^{18}$ ~ $2^{25}$的数据得到，数据规模减半tp可能会提高20%左右）【需要进一步实验】
  - 不同平台对throughput影响较大。
    个人笔记本比服务器快。
  - 不同数据setting下对throughput可能会有影响。如大量插入+大量删除可能会造成load unbalanced。
  - 重构tp【未实现】

- **Bit per key：空间指标**

  - Static version：FIlter总共使用bit数量 / Filter最大可装载的key数量
  - Dynamic version：Filter目前allocate的bit数量 / Filter目前装载的key数量。
    相比于static case，dynamic的bpk也是dynamic的。
    【如何设计动态空间使用的指标？例如$\frac{1}{T} \int bpk(2^t) \text{d} t$，其中$bpk(n)$表示插入$n$个key之后的bpk】

  - **逻辑上 / 实际上**：实际实现的算法的bpk可能由于实际中的限制（如最小单位1byte=8bit，或实现上的偷懒）比逻辑上算法设计的bpk大。

    - 别人实现的filter是否有这种情况？**实际bpk matters！**

      但是实现出理论bpk的算法会带来效率上的overhead，而且会变得更复杂。【未实现】

- **False positive rate：精确度指标**

  - **State dependent！** 在filter的某个状态时，等概率随机询问一个key $k \in \mathcal{K}$（key空间可以被认为无限大）后，filter回答在集合内的概率。
    初始时fpr为$0$，集合大小$|S|$越大，fpr越大。

  - **Theory bound！** 任何filter都将无限大的key空间映射到了有限的空间内。
    对于我们的filter，raw fingerprint的长度$r$准确刻画了fpr。假设插入了$n$个随机的key后：
    $$
    fpr = 1-(1-\frac{1}{2^l})^n
    $$

  - fpr与空间的tradeoff：似乎$-\log_2 fpr$是更好的评价标准。

    对于每个key多记一位fingerprint，fpr约除以$2$。

## Our Algorithm

- **2-filter：**
  - 由若干个表（或*表段*）组成，每个key恰好映射到两个桶里（两个桶必定在不同的表里），但是插入时的key只被存在映射到的恰好一个桶中。

- **Raw Fingerprint**：

  - 一个key $k \in \mathcal{K}$的raw fingerprint为$H(k)$。

    哈希函数$H : \mathcal{K} \to \mathcal{R}$。其中$\mathcal{R} = uint_{r}$表示raw fingerprint的空间，即**所有$r$位无符号二进制数**。

  - 这是filter区分key使用的唯一标识。对于两个具有相同raw fingerprint的key，filter对它们不做区分。

  - $r$是**事先**确定好的，无法改变。（因为不能将key存储下来，理论上不能实现动态版本one-pass的低fpr filter）

- **Bucket** 桶：

  - 表的组成部分。
  - **Intuition：** 桶作为一个小Filter，支持插入/删除/**准确的**查询。每个桶只负责部分raw fingerprint，且一个表里的每个桶负责的部分应该是整个$\mathcal{R}$的partition。（详情见表部分）
  - 桶内插入/删除/查询的key被称为fingerprint（经典的称法）。直接使用raw fingerprint作为key正确性是对的，但是可以使用更短的fingerprint节省空间。（详情见表部分）
  - 利用局部性，使用了暴力的算法。可以进行Loop unrolling / SIMD优化。【需要优化】
  - **容量：** 一个桶内最多能维护的元素个数。经典算法中为slot个数，如cuckoo中固定为$4$。
    我们的算法目前大多数为$8$，且可以通过 <u>[capacity stealing]</u> 的trick进行扩容。

- **Table** 表：

  - 由若干个桶组成，将每个raw fingerprint恰好映射到其中一个桶内。一般来说，第$l$层表具有$2^l$个桶，$l$被称作表的层数（**level**）。每个桶在内存是连续存储的。
    **分配函数** $D: \mathcal{R} \to [0, 2^l)$，将raw fingerprint映射到桶的下标，每个桶作为小filter只需要负责分配到的raw fingerprint的集合的维护。

  - **Intuition：**本质是一种Data partition。若分配后超出桶容量，处理会比较棘手。
    最Trivial的哈希表相当于Data partition到每个容量为$1$的桶内，但是Collision非常严重。
    Cuckoo哈希相当于Data partition到容量为$4$的桶内，且使用互相kick做Load balancing。
    我们使用Data partition到容量为$8$的桶内，且使用不同表之间kick做load balancing。

  - **类型：** 桶的类型由分配函数决定。目前的算法中，存在两种不同类型（分配函数）：

    - **High bit distribution：**设$x\in uint_r$为$r$位的raw fingerprint，则$D(x) = \lfloor \frac{x}{2^{r-l}}\rfloor$取$x$的$l$位高位。

      分配后，剩下的$r-l$位低位作为分配到的桶的fingerprint（即小filter的key）。

    - **Low bit distribution：** 设$x \in uint_r$为$r$位raw fingerprint，则$D(x) = x \bmod 2^{l}$取$x$的$l$位低位。

      分配后，剩下的$r-l$位高位作为分配到的桶的fingerprint（即小filter的key）。

    使用High bit distribution的称为**高表**。

    使用Low bit distribution的称为**低表**。

  - **性质：**桶的下标+桶内存储的fingerprint 可以完全还原出 raw fingerprint

- **Table segment** 表段：

  - 一个完整的第$l$层表的某一段桶所组成的表，用于动态算法中，每个表段可以独立扩容（重构），且扩容时具有局部性。
  - **Intuition：** 对于$\mathcal{R}$的划分$\mathcal{R} = \mathcal{R}_0 \cup \mathcal{R}_1 \cup \dots \cup \mathcal{R}_{s-1}$，第$i$个表段只负责$\mathcal{R}_i$中的raw fingerprint。这样每个表段可以独立扩容（例如层数加一）。
  - 动态算法中分为$s=32$段，第$l$层表的第$i$段表段是由下标在[$ i \cdot(2^l /32), (i+1) \cdot(2^l/32))$中的桶组成的。
    对于$l$层高表的第$i$段，它所负责的是所有raw fingerprint最高五位为$i$的部分。
    对于$l$层低表的第$i$段，它所负责的是所有raw fingerprint最低五位为$i$的部分。（假设总位数$r \ge 5$）

- **2-filter algorithm：**

  - Static version：

    维护第$l$层高表 与 第$l$层低表。
    对于插入：使用**插入策略**与**平衡策略**决定插入到高表还是低表，并且进行高表和低表之间的load balancing。

    对于查询：使用**查询策略**决定优先查询哪个表。【【**new**】查询时是否可以顺带load balancing？】

    对于删除：使用查询操作定位后删除。【删除后是否需要load balancing？】

  - Dynamic version：

    维护高表表段 与 低表表段 ，其中段数$s = 32$。

    与静态版本类似，但是当**插入策略**决定对某个表段进行扩容时，需要使用**扩容策略**将表段的层数提升。

  - 以上给定的为算法框架，具体细节与优化应当在 插入/平衡/查询/扩容 策略中决定。

- **Policies:**

  [Old] 表示原大表小表设计时的策略。**目前代码实现是Old**。Old其实是新的算法框架的一个特例，即高表和低表保持层数差一。

  [Primary] 表示每个raw fingerprint设定一个**优先位置**，即优先放在高表还是优先放在低表。

  [One] 表示raw fingerprint插入时，先看的桶如果负载低于平均值（或别的）则直接插入，无需查看另一个桶。

  - 插入策略：
    - [Old] 大表小表：看两个桶哪个小就先插哪个。（配合[Old]平衡策略，多插小表可以用效率tradeoff load factor）
    - [Primary + One] 优先看高表，若高表的桶低于平均值则直接插入，否则选择较小的。
  - 平衡策略：
    - [Old] 只能从小表踢到大表。
    - [Primary + One] 只能踢一次。
  - 查询策略：
    - [Old] 优先查大表。
    - [Primary + One] 优先查优先位置。
  - 扩容策略：
    - 踢不了就扩容。

  **目前的策略对于动态情况不太适用，无论是Old还是Primary+One。**

- **Tricks：**

  - **Capacity Stealing**：
    - 在内存中，每个桶容量为$8$，且各自存储空间各不相交。但是当相邻（左/右）的桶容量有剩余时，可以借用相邻的桶的容量。这样自己的容量加一，相邻的桶的容量减一。
  - 

