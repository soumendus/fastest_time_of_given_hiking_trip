# fastest_time_of_given_hiking_trip
### This Program calculates the fastest time of the given hiking trip.

Write a C++ program that simulates a team hiking through a forest at night. The team
encounters a series of narrow bridges along the way. At each bridge they may meet additional
hikers who need their help to cross the bridge.
The narrow bridge can only hold two people at a time. They have one torch and because it's
night, the torch has to be used when crossing the bridge. Each hiker can cross the bridge at
different speeds. When two hikers cross the bridge together, they must move at the slower
person's pace.

Determine the fastest time that the hikers can cross each bridge and the total time for all
crossings. The input to the program will be a yaml file that describes the speeds of each person,
the bridges encountered, their length, and the additional hikers encountered along the way.
Your solution should assume any number of people and bridges can be provided as inputs.
Demonstrate the operation of your program using the following inputs: the hikers cross 3
bridges, at the first bridge (100 ft long) 4 hikers cross (hiker A can cross at 100 ft/minute, B at 50
ft/minute, C at 20 ft/minute, and D at 10 ft/minute), at the second bridge (250 ft long) an
additional hiker crosses with the team (E at 2.5 ft/minute), and finally at the last bridge (150 ft
long) two hikers are encountered (F at 25 ft/minute and G at 15 ft/minute).


# 1. Strategy(s) - there are several ways to solve the problem, you can provide more thanone. The goal is to show us how you think.


I have devised concurrent approach to solve this problem. Activity at each bridge is an independent task which has no dependency and it can be run on a independent worker node of a cluster. Each worker node can write its result either to a shared memory which the Master node can also access or else the worker node can write its result calculating it independently to a file name say "result.out" which is in a shared filesystem which the master node can also access. Or else the worker nodes can write their result using RDMA(Remote Direct Memory Access) to the Master's node memory and the master node can read it from there. In this program, I have created Master node program and worker node program. The master node delegates work to the worker nodes in the cluster and once the result of the worker node is available, the master node calculates the total time of the trip. Since I don't have a real cluster or a resource manager to send jobs, I have simulated that using multi-threading on a SMP(Symmetric Multiprocessing) system. To simulate, maximum parallelism, I have avoided locks where it is not needed.



# 2. Architecture and design- we want to see how well you architect and design solutions to complex problems.

I have used Concurrent programming. Master node delegates work to worker nodes in the cluster and finally calculates the result after all the worker nodes completes their part of job.The worker nodes work all independently in a parallel manner
so execution happens faster.


# 3. Testing - we want to see how you approach testing of the solution.
The following testing approach I am following.


(a) Test each modules based on the Test Description Specification i.e check the observed behavior, with respect to the expected behavior. Create a C++ test driver which reads test values from the input file and feeds the module under test.
   
   - Master Node Functionality
          - File handling
          - Parser module - I have implemented my own YAML parser and I have not used the standard parser so it needs
          good amount of testing.
          - Creating lists for the worker node.
          - Creation of the threads for the worker nodes.
   
   
     - Worker Node functionality
          - Check the calculation with different inputs.
   
   
   (b) Do stress testing which tries to stress the system by providing input which stresses the CPU, Memory, Network etc.
   I will not be using network testing as I am simulating this on a single node but on a real scenario, we need to also
   test the behavior of the network with respect to the test data. Basically check the breaking point of the developed
   application.
   
   
   (c) I would use proper logging in the code that I have written, to write program messages to a different file. This file
   can be analyzed later to check the behavior of the developed application.
   


# 4. Standards and best practices.

Indent the code and document it. I have done that. Organize all the classes to a header file and the function definitions to CPP files. Currently code organization is pending. I will do it eventually. I only got a few hours to work in Saturday and some time to upload the code in Sunday. Did not get much time.


# 5. Explanation

I have used data structures like Priority Queues to find the torch bearer person who has the maximum speed so that he 
can cover the distance walking alone to escort the guys with the torch in the bridge faster. Also to simulate the crossing of the bridge, I have used C++ Queue Data Structures. I have also used C++ List and vector STL.


## Demonstrate the operation of your program using the following inputs: the hikers cross 3 bridges, at the first bridge (100 ft long) 4 hikers cross (hiker A can cross at 100 ft/minute, B at 50 ft/minute, C at 20 ft/minute, and D at 10 ft/minute), at the second bridge (250 ft long) an additional hiker crosses with the team (E at 2.5 ft/minute), and finally at the last bridge (150 ft long) two hikers are encountered (F at 25 ft/minute and G at 15 ft/minute).

As per the my program and calculation, my program calculates this with maximum paraellism and the following is the result.

Time to cross the First Bridge = 19.000000 

Time to cross the Second Bridge = 150.000000 

Time to cross the Third Bridge = 109.000000 

Total fastest Time for the Entire trip =  278 minutes

I have used two methods using shared memory and writing to a file in a shared File system. In both methods, the result which I get is 278 minutes.
