# chiSIM #

Welcome to the github repository for the Chicago Social Interaction Model (chiSIM) framework.

chiSIM is a framework for implementing agent-based models that simulate the mixing of a synthetic population. In a chiSIM based model,
each agent, that is, each person in the simulated population, resides in a place (a household, dormitory or retirement home/long term care facility, for example),
and moves among other places such as workplaces, homes, clinics, and community resources. Agents typically move between places according to their domain specific activity profile, such that each agent has a profile that determines at what times throughout the day they occupy a particular location. Once in a place, an agent mixes with other agents in some model or domain-specific way. For example, an agent may expose other agents to a disease in an epidemiological model.

chiSIM itself is a generalization of a model of community associated methicillin-resistant Staphylococcus aureus (CA-MRSA) (Macal et al. 2014). The CA-MRSA model was a non-distributed model in which all the model components, including all the agents and places, ran on a single computational process. chiSIM retains and generalizes the social interaction dynamics of the CA-MRSA model and allows models implemented using chiSIM to be distributed across multiple processes. Places are created on a process and remain there. Persons move among the processes according to their activity profiles. When a person agent selects a next place to move to, the person may stay on its current process or it may have to move to another process if its next place is not on the person’s current process. A load balancing algorithm can be applied to the synthetic population to create an efficient distribution of agents and places, minimizing this computationally expensive cross-process movement of persons and balancing the number of persons on each process (Collier, Ozik, and Macal 2015).

For more information on compiling and using the framework, see the [Users Guide](./users_guide.md).

## References ##

Macal, C. M., M. J. North, N. Collier, V. M. Dukic, D. T. Wegener, M. Z. David, R. S. Daum, P. Schumm, J. A.
Evans, J. R. Wilder et al. 2014. “Modeling the transmission of community-associated methicillin-resistant Staphylococcus aureus: 
a dynamic agent-based simulation”. *Journal of translational medicine* 12 (1): 124.

Collier, N. T., J. Ozik, and C. M. Macal. 2015. “Large-Scale Agent-Based Modeling with Repast HPC: A Case Study in Parallelizing an Agent-Based Model”. 
In *Euro-Par 2015: Parallel Processing Workshops - Euro-Par 2015 International Workshops, Vienna, Austria, August 24-25, 2015, Revised Selected Papers*, 454–465.

