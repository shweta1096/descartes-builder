TODO Clean-up this description...

In FDF, we can leverage the fact that functions are first-class citizens
to infer extra information about them. We explain in this section how to
automatically infer (implicit) types for the data and functions
generated within the pipeline, based on the FDF syntax and semantics.

Implicit typing endows the FDF pipeline with advantages commonly
associated with statically typed languages, e.g. being less prone to
errors and easier to
maintain [@bognerTypeNotType2022; @rayLargeScaleStudy2014]. The typing
is *implicit*, that is, it does not require the user to manually define
explicit types, which can be
laborious [@oreAssessingTypeAnnotation2018], and even infeasible in the
case of DT design due to the dynamic nature of the functions learned
within the pipeline. For instance, when applying PCA to obtain a reduced
basis preserving 99% of the variance of the original dataset, the
explicit output type (i.e., number of dimensions) depends on the
training data: explicit typing would not be feasible in this case.

Implicit Types
--------------

First, for all (data or function) port
$i \in \FDports = \{1, \ldots, m\}$, we define $\default(i)=i$. Now, we
associate an *implicit* data type $\type(p)$ to each  and to each .

For *s*, we define the set $\typeset = \{1, \ldots, m\}$ of Data Types,
which is the same as the set of ports $\FDports$. By default, port $p$
has implicit type $\type(p) \gets \default(p)$, but it may be given type
$\type(p) \gets \default(q)< \default(p)$ if it is known that the types
of ports $p,q$ are the same. In general, some of the numbers in $[1, m]$
will not be used, as several ports will have the same implicit type.

For *Function ports*, the set of function types is defined as
$\functypeset = \cup_{i \in \mathbb{N}} \typeset^i \times \cup_{j \in \mathbb{N}} \typeset^j$.
That is, a function $f$ with $k$ inputs and $k'$ outputs will have the
implicit type $((\type_{1}, \ldots, \type_{k}), (\type_{k+1},$
$\ldots , \type_{k+k'}))$, where $\type_{i \leq k}$ is the implicit type
of the $i$-th input, and $\type_{k+i}$ is the implicit type of the
$i$-th output of $f$ for $i\leq k'$.

By default, a *Coder* box $\FDbox$, with $\type_{1}, \ldots, \type_{k}$
denoting the types of s of $\FDbox$, generates:

-   a function Encode of type
    $((\type_{1}, \ldots, \type_{k}),(\type_{Out}))$. $\type_{Out}$ is a
    fresh type never seen before and represents the data on the reduced
    basis, and

-   a function Decode of type
    $((\type_{Out}),(\type_{1}, \ldots, \type_{k}))$.

This default can be changed by providing extra information in the
library containing the predefined function specified by the Coder's
parameter. For example, if the parameter of $\FDbox$ calls a
normalization procedure, we could have $\type_{Out} = \type_{1}$.

A *Trainer* box $\FDbox$ with $\ell$ s, generates, by default, a
function Predict of type
$((\type_1, \ldots, \type_k),(\type_{k+1}, \ldots,\type_{\ell})),$ where
$\type_{1}, \ldots, \type_{\ell}$ are the types of the s of $\FDbox$,
and $k$ is the number provided in the first component of the parameter
of the Trainer box $\FDbox$. This default can also be changed in the
library.

Type propagation and checking
-----------------------------

We now explain how to propagate the types automatically between ports.
The types are propagated via the FDF Graph topological order. We assume,
without loss of generality, that the port numbering follows the
topological order.

Note that the type checking may return warnings to the user if it does
not have enough information to ensure that the two types are equal. In
this case, the user would either confirm that the two ports have the
same type or rectify the pipeline if the type mismatch is genuine. The
user can also add explicit *type annotations* before running the type
checking, to provide this information. Specifically, if different ports
$\FDport_1, p_2, \ldots, p_s$ have the same annotation (except
exponents), then they have the same implicit type. Thus, we can set
$\type(p_{1}) = \ldots = \type(p_{s}) \gets min_{j \leq s}(\type({p_j}))$.

The type propagation proceeds in three main steps. The *first step* is
to propagate types for the DataIO . Let $\{1\ldots, r\}$ be the s of
DataIO (that is, the first $r$ ports of the FDF pipeline). By default,
each port $i \leq r$ will have a different data type:
$\type(i) \gets \default(i) = i$. The user may add annotations to
specify otherwise.

The *second step* is to propagate the types through the ports which are
associated with a box $b$. First, each  $p \in \FDports^I$ with
$\FDportBox(p)=b$ copies the implicit data type from the corresponding
 $\FDinPoutP(\FDport)$, that is
$\type(\FDport)\gets \type(\FDinPoutP(\FDport))$.

Finally, the *third step* is to compute the implicit type for each  of
$\FDbox$. Note that, in general, an  $p$ can either have its default
type, $\type(p) = \default(p)$, or it can have a type
$\type(p) = \type(q) < \default(p)$, propagated from a previous port $q$
(through possibly several boxes). The implicit type depends on
$\FDboxType(\FDbox)$ as follows.

### Coder Type Propagation

Let $\FDport_{Out}$ be the   of $\FDbox$ (if there are two output
function ports, take the minimal $\default(\FDport)$: there is at least
one ). Then, a box $\FDbox$ with $\FDboxType(\FDbox)=$ Coder has
$\default(\FDport_{Out})$ as the output type of Encode (and the input
type of Decode). This guarantees by construction that this type has not
been used before. Let $\type_1, \ldots, \type_r$ be the implicit types
of the s of $\FDbox$, and let $p_{\text{Encoder}}$, $p_{\text{Decoder}}$
be the two output function ports. We define: $$\begin{aligned}
    \type(p_{\text{Encoder}}) &\gets ((\type_1, \ldots, \type_r),(\default(\FDport_{Out}))) \\
    \type(p_{\text{Decoder}}) &\gets ((\default(\FDport_{Out})),(\type_1, \ldots, \type_r)) \end{aligned}$$

### Trainer Type Propagation

A box $\FDbox$ with $\FDboxType(\FDbox)=$ Trai-ner has a single . We
define its implicit type as follows. Let $k$ be the number provided by
Param; let $\type_1, \ldots, \type_\ell$ be the implicit types of the s
of $\FDbox$; let $\FDport$ be the  of $\FDbox$. Then:
$$\type(\FDport) \gets ((\type_1, \ldots, \type_k),(\type_{k+1}, \ldots, \type_\ell))$$

### Processor Type Propagation

For $\FDboxType(\FDbox)=$ Processor, we have two cases. The *first case*
is when $\FDbox$ has a  $\FDport_F$. We denote
$\type(\FDport_F)= ((\type_1, \ldots,\type_k),\allowbreak(\type'_{1}, \ldots, \type'_{k'}))$.
Before propagating the type, we must ensure the following conditions are
absent. If a condition is detected, we raise an error or warning:

1.  *Mismatch in the number of input/output.* A mismatch error occurs if
    the number of s of $\FDbox$ is not $k$ or if the number of s of
    $\FDbox$ is not $k'$. The user should fix the pipeline.

2.  *Inconsistent input type.* An inconsistent input type warning occurs
    when $\exists j \in [1, k]: \type(\FDport_j) \neq \type_j$, i.e.,
    the type expected as input by $\FDport_F$ does not match the type of
    port $p_j$. To fix this inconsistency, the user can either tell that
    the two types are identical (for instance, by annotation) or fix the
    pipeline.

If no such problem is encountered, we can set
$\type(\FDport_{k+j}) \gets \type'_{j}$ for all $j \leq k'$, and
propagate to the next boxes.

The *second case* is when $\FDbox$ has no , but instead, its parameter
specifies a predefined function PredefFunc from a library. The library
is unaware of the implicit types propagated in one particular FDF
pipeline. Further, a function from a library can be polymorphic,
accepting several types as input, another reason to not impose strong
typing. The library can provide however weak type information: first,
its number $k$ of input ports and $k'$ of output ports (If the input
vectors can take any size, then the input is multiplexed into a single
port and $k=1$; similarly for the output). The library can also specify
a partition $P_1, \ldots, P_r$ with
$P_1 \sqcup P_2 \sqcup \cdots \sqcup P_r$ of the set of all (input and
output) ports of the function. Each partition represents the fact that
types should be equal within the partition.

Similar to the first case, we must first ensure the following two
conditions are absent:

1.  Mismatch in the number of inputs or outputs.

2.  Inconsistent input type. This condition occurs if two ports from the
    same partitions have different types, i.e., if
    $\exists i \leq r: (p, q) \in P_i\text{ and }\type(p)\allowbreak\neq\type(q)$.

If a warning is raised, the user can fix it as in the first case, e.g.
by providing the information that the two types are the same. Once there
are no more warnings or errors, the type propagation proceeds as
follows. For all $i \leq r$, either:

1.  there is an  $p \in P_i$, and then we set $\type(q)\gets\type(p)$
    for all the output ports $q$ in $P_i$, as the library specified that
    these output data ports have the same implicit type as the input
    data port $p$ [^1], or

2.  there is no such , and then we set a fresh
    $\type(q)\gets\min_{q \in P_i}\default(q)$ for all the (output)
    ports $q$ in $P_i$.

[^1]: Observe that the condition checking ensures that all the data
    ports in $P_i$ have the same type.
