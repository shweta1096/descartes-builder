## Tasks
There are three user-specified boxes in FDF, each associated with a
different task in the twinning workflow:
-   Processor: For typical data processing (including applying functions learned by other boxes),
-   Coder: For learning a reduced basis (or unsupervised clustering) and the associated projection and inverse projection, and
-   Trainer: For learning a function with supervised ML.

## Box syntax 
The inputs and outputs of the boxes above provided via _ports_. We define detailed the syntax of each box in below.

A __Processor__ box accepts data and return data:
- Multiple ($k \geq 1$) inputs to receive the data for processing.
- Multiple ($k' \geq 1$) outputs to return the processed data.
- Function to execute provided via input port, or pre-defined in the Builder library or directly provided by the user.

A __Coder__ box accepts data and return two functions: 
- Multiple ($k \geq 1$) inputs to receive data from which to compute the reduced basis.
- One or two outputs to return encode and decode functions (i.e., the projection and inverse
projection onto the reduced basis, respectively). 
- Function (i.e., algorithm to obtain encode/decode) to execute pre-defined in the Builder library, or directly provided by the user. 

A __Trainer__ box accepts data and return one function: 
- Two or more ($\ell \geq 2$) inputs to receive the supervised $(X, Y)$ pairs. The first $k < \ell$ provide $X$ and the remaining $k' = \ell - k$ provide $Y$. The number $\ell$ must be provided as a parameter to the box. 
- One output to return the predict function that predicts $Y$ given $X$.
- Function (i.e., algorithm to obtain predict) to execute pre-defined in the Builder library, or directly provided by the user. 

## Parameters
Any parameters required for the Box are provided via a YAML file, `parameters.yml`. Global parameters `gparam` can be provided as well as parameters associated to a box with `box_name`. 

/!\ box_name param not yet implemented. 
```
gparam1: gvalue1
gparam2: gvalue2
box_name:
    param1: value1
    param2: value2
    ...
```

## Execution 
The boxes are executed per the directed graph representing the pipeline. This graph must be a directed graph, and it can be defined as follows (we employ Kedro routines to define it under-the-hood). There is:
- an edge `p -> q`, where `p` is an *output* port, `q` is an *input* port and `p = src(q)`, that is `q` is the source of the data/function for `p`.
- an edge `p -> q`, where `p` is an *input* port, `q` is an *output* port and both `p` and `q` are associated with the same box. 

TODO execution consists et
