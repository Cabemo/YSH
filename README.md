# Yet Another Shell (ysh)

>Based on the [Kamalmarhubi's Workshop](https://github.com/kamalmarhubi/shell-workshop). Thanks for setting the road!

## Description

Many of us programmers do not wonder how many of the peculiarities within a shell that we use day to day pass without us thinking how it is implemented. The main problem to be solved with this project is to confront this gap between my knowledge of how to use a shell and how is implemented. The problem may be broken into the following pieces:

- Input parsing
- Running program
- Implement pipelines
- IO Redirection
- Environment variables
- Background processes
- Global expressions
- Job handling (SIGINT, SIGSTOP and SIGCONT)

## Solution

<!-- With some [research](https://brennan.io/2015/01/16/write-a-shell-in-c/) I have found that the main process for a shell is the following:

\begin{center}
\begin{tikzpicture}[shorten >=1pt,node distance=3cm,on grid,auto]
	\node[state, initial] (0) {$Read$};
	\node[state] (1) [right=of 0] {$Parse$};
	\node[state, accepting] (2) [right=of 1] {$Execute$};
	\path[->]
		(0)
			edge [bend left] node {Input} (1)
			edge [loop above] node {No input} (0)
		(1)
			edge [bend left] node {Invalid input} (0)
			edge [bend left] node {Command} (2)
		;
\end{tikzpicture}
\end{center} -->

On each step in the process there are certain deviations that can be taken (e.g. multiple piped commands, io redirection, etc...). In the following paragraphs I'll explain the functionalities and broad explanation of how each one will be taken care of.

### Input parsing

Maybe the most exhausting task of the process, input parsing, takes a line from the input and does the following:

	- Split by tokens
	- Determine if the program has pipelines
	- Determine if the program has redirections

### Running program


### Implement pipelines


### IO Redirection


### Environment variables


### Background processes


### Global expressions


### Job handling (SIGINT, SIGSTOP and SIGCONT)


## Concept specific explanation

### Dynamic memory

This topic will be specifically implemented in many of the necessary variables such as the **pipes** that must be saved on the run.

### Pointers

As with any `struct`, pointers will be used for `struct glob_t` and any other that are needed.

### Process creation

Needless to say a shell is nothing without process creation, therefore `fork` will be an essential part of the project/program.

### Inter process communication

The IPC will be managed mainly with pipes due to the nature of them. I'll be considering in which way I can use sockets as it's a topic that I personally liked very much, but I do not know at the moment of any specific implementation that can be used within a shell.

### Signals

Finally, signals are a core theme within a shell as seen in the [Job handling](#job-handling-sigint-sigstop-and-sigcont) section. Therefore signals will definitely be used in order to manage communication between processes executed and jobs.