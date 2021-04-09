sc-pseudo.exe
=============

sc-pseudo.exe is a recreation of Windows Service Control Manager, a command line utility. This code was built using a 64 bit architecture. 
This script generates a system process that allows Windows to start, stop and interact with other processes. 

If required parameters are not passed to the commands, sc-pseudo will print a help statement to screen.

### Execution Examples
Here are some examples of how to execute the code on an Adminster Command Prompt terminal:
 <pre><code>sc-pseudo.exe --create test --binpath=C:\Users\green_pastures\Desktop --displayname=Greens_Midterm_Deserves_An_A</code></pre>
 <pre><code>sc-pseudo.exe --query</code></pre>
 <pre><code>sc-pseudo.exe --query service_name</code></pre>
 <pre><code>sc-pseudo.exe --delete test --binpath=C:\Users\green_pastures\Desktop</code></pre>

### Options Commands:
- query-----------Queries the status for a service, or
                  enumerates the status for types of services
- start-----------Starts a service
- stop------------Sends a STOP request to a service
- config----------Changes the configuration of a service (persistant)
- description-----Changes the description of a service
- failure---------Changes the actions taken by a service upon failure
- qdescription----Queries the description for a service
- delete----------Deletes a service (from the registry)
- create----------Creates a service. (adds it to the registry)
            
### Optional query switches 
 - type---------Specifies the type of services or type of drivers to be enumerated
 - state--------Specifies the started state of the service to be enumerated
 - bufsize------Specifies the size (in bytes) of the enumeration buffer
 - ri-----------Specifies the index number at which enumeration is to begin or resume.
 - group--------Specifies the service group to be enumerated.

### Optional create & delete switches 
- start----------Specifies the start type for the service.
- error----------The severity of the error, and action taken, if this service fails to start.
- binpath--------Specifies a path to the service binary file. 
- group----------Specifies the name of the group of which this service is a member.
- tag------------Specifies whether or not to obtain a TagID from the CreateService call.
- depend---------Specifies the names of services or groups that must start before this service. The names are separated by forward slashes (/).
- obj------------Specifies a name of an account in which a service will run, or specifies a name of the Windows driver object in which the driver will run.
- displayname----Specifies a friendly name for identifying the service in user interface programs.
- password-------Specifies a password. This is required if an account other than the LocalSystem account is used. 

### Optional failure switches
- actions--------Specifies one or more failure actions and their delay times (in milliseconds), separated by a forward slash (/).
- reboot---------Specifies the message to be broadcast when a service fails.
- command--------Specifies the command-line command to be run when the specified service fails.
- reset----------Specifies the length of the period (in seconds) with no failures after which the failure count should be reset to 0 (zero).
