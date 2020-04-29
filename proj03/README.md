## Functional Decomposition

Solution to [Project #3](http://web.engr.oregonstate.edu/~mjb/cs575/Projects/proj03.html) 
(Functional Decomposition).

This is an attempt to use parallelism for programming
convenience. A month-by-month simulation was created in which each agent
of the simulation will execute in its own thread where it just has to
look at the state of the world around it and react to it. The simulation
involves four agents.

-   Deer consumes grains. A new deer enters the environment every month
    if every deer present so far can get grain. A deer leaves the group
    every time this is not possible in the herd.

-   Grain grows based on the environment state and the number of humans
    available for farming in the environment.

-   Humans **(My Agent)** feed on grains. They sells one deer each by
    the end of the year if available. Every time the entire group is
    able to sell a deer or is able to feed themselves a new human enters
    the environment. Every time either of the above is not possible a
    human leaves the environment.

-   Watcher prints the current state, increases the month and so the
    year (when applicable), and updates the environment state based on
    the new month number.
    
Other details can be found in parent [README.md](https://github.com/sudharkj/cs575/blob/master/README.md)
