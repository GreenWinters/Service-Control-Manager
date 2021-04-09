/*
Author: Bahirah Adewunmi
March 16 - April 8, 2021
Recreation of Service Control Manager-sc.exe [version 2.1]


For more information on error codes visit:https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes
*/

#include <windows.h>
#include <stdio.h>
#include <cstdlib>
#include <WinSvc.h>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>

using namespace std;
namespace po=boost::program_options;

int main(int argc, const char *argv[])
{
    SC_HANDLE schSCManager;
    string service_name;
    string  binary_path;
    string  loadordergroup;
    string  depend;
    string  obj; 
    string  display_name;
    string  password;
    string reboot;
    string command;
    string actions;

    try
    {
        po::options_description desc("Switches");
        desc.add_options()
            ("query", po::value<string>(&service_name)->implicit_value(""), "query") 
            ("create", po::value<string>(&service_name)->implicit_value(""), "Create a service. (add it to the registry)")
            ("qdescription", po::value<string>(&service_name), "Queries the description for a service.")
            ("start", po::value<string>(&service_name), "start service")
            ("delete", po::value<string>(&service_name), "Delete a service (from the registry)")
            ("config", po::value<string>(&service_name), "permanently change the service configuration")
            ("failure", po::value<string>(&service_name), "Change the actions taken by a service upon failure")
            ("stop", po::value<string>(&service_name), "Sends a STOP request to a service.")

            //query switches 
            ("type=", po::value<string>(), "Specifies the type of services or type of drivers to be enumerated")
            ("state=", po::value<string>(), "Specifies the started state of the service to be enumerated.")
            ("bufsize", po::value<int>(), "Specifies the size (in bytes) of the enumeration buffer.")
            ("ri=", po::value<int>(), "ri")
            ("group=", po::value<string>(), "group")

            //create switches + type
            ("start=", po::value<string>(), "Specifies the start type for the service.")
            ("error=", po::value<string>(), "The severity of the error, and action taken, if this service fails to start.")
            ("binpath=", po::value(&binary_path)->default_value(""), "Specifies a path to the service binary file.")
            ("group=", po::value(&loadordergroup)->default_value(""), "Specifies the name of the group of which this service is a member")
            ("tag=", po::value<string>(), "Specifies whether or not to obtain a TagID from the CreateService call")
            ("depend", po::value(&depend)->default_value(""), "Specifies the names of services or groups that must start before this service. The names are separated by forward slashes (/)")
            ("obj=", po::value(&obj)->default_value(""), "Specifies a name of an account in which a service will run, or specifies a name of the Windows driver object in which the driver will run")
            ("displayname=", po::value(&display_name)->default_value(""), "Specifies a friendly name for identifying the service in user interface programs")
            ("password=", po::value(&password)->default_value(""), "Specifies a password. This is required if an account other than the LocalSystem account is used.")

            //failure switches
            ("actions=", po::value<string>(&actions)->default_value(""), "Specifies one or more failure actions and their delay times (in milliseconds), separated by a forward slash (/)")
            ("reboot=", po::value<>(&reboot), "Specifies the message to be broadcast when a service fails.")
            ("command=", po::value<>(&command), "Specifies the command-line command to be run when the specified service fails")
            ("reset=", po::value<int>()->default_value(2), "Specifies the length of the period (in seconds) with no failures after which the failure count should be reset to 0 (zero)")
            ;

        po::command_line_parser parser{ argc, argv };
        parser.options(desc).style(
            po::command_line_style::default_style |
            po::command_line_style::allow_slash_for_short);
        po::parsed_options parsed_options = parser.run();

        po::variables_map vm;
        po::store(parsed_options, vm);
        po::notify(vm);
    
        //QUERY
        if (vm.count("query"))
        { 
            //Open Service Control Manager
            schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS); //All possible access rights

            if (schSCManager == NULL)
            {
                printf("EnumQueryServicesStatus:OpenService FAILED %u\n", GetLastError());
                printf("\n\nThe specified service does not exist as an installed service");
                return 1;
            }


            //Obtains and displays information about the specified service, 
            //driver, type of service, or type of driver.

            // cout << "Test: Query was passed";

            //returns the most recent service 
            //status information reported to the service control manager.
            DWORD service_type;
            if (vm.count("type="))
            {
                if (vm["type="].as<string>() == "all")
                {
                    service_type = (SERVICE_WIN32 | SERVICE_DRIVER);
                }
                else if (vm["type="].as<string>() == "driver")
                {
                    service_type = SERVICE_DRIVER;
                }
                else if (vm["type="].as<string>() == "services")
                {
                    service_type = SERVICE_WIN32;
                }
                else if (vm["type="].as<string>() == "own")
                {
                    service_type = SERVICE_WIN32_OWN_PROCESS;
                }
                else if (vm["type="].as<string>() == "share")
                {
                    service_type = SERVICE_WIN32_SHARE_PROCESS;
                }
                else if (vm["type="].as<string>() == "kernel")
                {
                    service_type = SERVICE_KERNEL_DRIVER;
                }
                else if (vm["type="].as<string>() == "filesys")
                {
                    service_type = SERVICE_FILE_SYSTEM_DRIVER;
                }
                else if (vm["type="].as<string>() == "interact type= own" || "own type= share" )
                {
                    service_type = (SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS);
                }
                else if (vm["type="].as<string>() == "interact type= share" || "share type= interact")
                {
                    service_type = (SERVICE_WIN32_SHARE_PROCESS | SERVICE_INTERACTIVE_PROCESS);
                }
                //Not doing the rec or adapt switches because they aren't defined in documentation
            }
            else
            {
                service_type = SERVICE_WIN32;
            }
            
            // state of service parameter
            DWORD service_state;
            if (vm.count("state="))
            {                
                if (vm["state="].as<string>() == "active") 
                {
                    service_state = SERVICE_ACTIVE;
                }
                else if (vm["state="].as<string>() == "inactive")
                {
                    service_state = SERVICE_INACTIVE;
                }
            }
            else
            {
                service_state = SERVICE_STATE_ALL;
            }

            // buffersize parameter
            DWORD buffsize;
            if (vm.count("bufsize=")) 
            {
                buffsize = vm["bufsize="].as<DWORD>();
            }
            else
            {
                buffsize = 1024;
            }

            // resume index parameter
            DWORD resumeindex; //Only relevant to enumerated ervices
            if (vm.count("ri="))
            {
                resumeindex = vm["ri="].as<DWORD>();
            }
            else
            {
                resumeindex = 0;
            }

            // group name parameter
            const char* groupname;
            if (vm.count("group="))
            {
                groupname = vm["group="].as<string>().c_str();
            }
            else
            {
                groupname = NULL;
            }


            //If no specific value passed, enumerate through all services
            if (vm["query"].as<string>()=="")
            {
                // First return the required size for enumerating the service query

                DWORD numServices = 0;
                DWORD spaceNeeded = 0;
               
                if (!EnumServicesStatusExA(schSCManager, SC_ENUM_PROCESS_INFO,
                    service_type, service_state,
                        NULL, 0, &spaceNeeded,
                        &numServices, &resumeindex, groupname)
                    && GetLastError() != ERROR_MORE_DATA)
                {
                    printf("\nEnumQueryServicesStatus:OpenService FAILED %d!!\n", GetLastError());
                    printf("\n\nThe specified service does not exist as an installed service.");
                    return 1;
                }

                spaceNeeded += sizeof(LPBYTE);
                ENUM_SERVICE_STATUS_PROCESSA* services_info = (ENUM_SERVICE_STATUS_PROCESSA*)malloc(spaceNeeded);


                //Enumerate through the services
                if (EnumServicesStatusExA(schSCManager, SC_ENUM_PROCESS_INFO,
                    service_type, service_state,
                    (LPBYTE) services_info, spaceNeeded, &spaceNeeded,
                    &numServices, &resumeindex, groupname))
                {
                    // Return all of the services and print them to the screen
                    for (DWORD i = 0; i < numServices; i++)
                    {
                        ENUM_SERVICE_STATUS_PROCESSA service = services_info[i];
                        //Open the service to get the rest of info to print to screen

                        // Get service handle for query 
                        SC_HANDLE service_handle = OpenServiceA(
                                                                schSCManager,         // SCM database 
                                                                service.lpServiceName, // name of service 
                                                                SERVICE_ALL_ACCESS);  // full access 

                        SERVICE_STATUS_PROCESS service_status_address;
                        DWORD bytes_needed; //number of bytes needed to store all status information

                        if (!QueryServiceStatusEx(
                            service_handle, // handle to service 
                            SC_STATUS_PROCESS_INFO, // info level
                            (LPBYTE)&service_status_address, // address of structure
                            sizeof(SERVICE_STATUS_PROCESS), // size of structure
                            &bytes_needed))              // if buffer too small
                        {
                            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
                            continue;
                        }
                        else
                        {
                            wcout << "SERVICE_NAME: " << service.lpServiceName << "\n";
                            wcout << "DISPLAY_NAME: " << service.lpDisplayName << "\n";
                            wcout << "\tTYPE\t\t\t: " << service_status_address.dwServiceType;
                            switch (service_status_address.dwServiceType){
                                case SERVICE_FILE_SYSTEM_DRIVER:
                                    wcout << "  FILE_SYSTEM_DRIVER\n";
                                    break;
                                case SERVICE_WIN32:
                                    wcout << "  WIN32\n";
                                    break;
                                case SERVICE_KERNEL_DRIVER:
                                    wcout << "  KERNEL_DRIVER\n";
                                    break;
                                case SERVICE_WIN32_OWN_PROCESS:
                                    wcout << "  WIN32_OWN_PROCESS\n";
                                    break;
                                case SERVICE_WIN32_SHARE_PROCESS:
                                    wcout << "  WIN32_SHARE_PROCESS\n";
                                    break;
                                case SERVICE_INTERACTIVE_PROCESS:
                                    wcout << "  INTERACTIVE_PROCESS\n";
                                    break;
                                default:
                                    wcout << "  " << service_status_address.dwServiceType << "\n";
                                    break;
                            }

                            wcout << "\tSTATE\t\t\t: " << service_status_address.dwCurrentState;

                            switch (service_status_address.dwCurrentState)
                            {
                            case SERVICE_RUNNING:
                                wcout << "  RUNNING\n";
                                break;
                            case SERVICE_STOPPED:
                                wcout << "  STOPPED\n";
                                break;
                            case SERVICE_CONTINUE_PENDING:
                                wcout << "  CONTINUING\n";
                                break;
                            case SERVICE_PAUSE_PENDING:
                                wcout << "  PAUSING\n";
                                break;
                            case SERVICE_PAUSED:
                                wcout << "  PAUSED\n";
                                break;
                            case SERVICE_START_PENDING:
                                wcout << "  STARTING\n";
                                break;
                            case SERVICE_STOP_PENDING:
                                wcout << "  STOPPING\n";
                                break;
                            default:
                                wcout << "  " << service_status_address.dwCurrentState << "\n";
                                break;
                            };

                            wcout << " \t\t\t\t ";
                            switch (service_status_address.dwControlsAccepted)
                            {
                            case SERVICE_ACCEPT_NETBINDCHANGE:
                                wcout << "( ACCEPT_NETBINDCHANGE )\n";
                                break;
                            case SERVICE_ACCEPT_PARAMCHANGE:
                                wcout << "( ACCEPT_PARAMCHANGE )\n";
                                break;
                            case SERVICE_ACCEPT_PAUSE_CONTINUE:
                                wcout << "( ACCEPT_PAUSE_CONTINUE )\n";
                                break;
                            case SERVICE_ACCEPT_PRESHUTDOWN:
                                wcout << "( ACCEPT_PRESHUTDOWN )\n";
                                break;
                            case SERVICE_ACCEPT_SHUTDOWN:
                                wcout << "( ACCEPT_SHUTDOWN )\n";
                                break;
                            case SERVICE_ACCEPT_STOP:
                                wcout << "( STOPPABLE )\n";
                                break;
                            default:
                                wcout << "( " << service_status_address.dwControlsAccepted << " )\n";
                                break;
                            };
                            wcout << "\tWIN32_EXIT_CODE\t\t: " << service_status_address.dwWin32ExitCode << "\n";
                            wcout << "\tSERVICE_EXIT_CODE\t: " << service_status_address.dwServiceSpecificExitCode << "\n";
                            wcout << "\tCHECKPOINT\t\t: " << service_status_address.dwCheckPoint << "\n";
                            wcout << "\tWAIT_HINT\t\t: " << service_status_address.dwWaitHint << "\n";
                            wcout << "\n";
                        }
                    }
                    CloseServiceHandle(schSCManager);
                }
                else
                {
                    printf("\nEnumQueryServicesStatus:OpenServiceA FAILED %d:\n\n", GetLastError());
                }
                
            }
            else
            {             
                //Convert string to const char* == LPCSTR
                const char* service_name_input = service_name.c_str();

                //Get service handle for query 
                SC_HANDLE service_handle = OpenServiceA(schSCManager, // SCM database 
                                                        service_name_input, // name of service 
                                                        SERVICE_ALL_ACCESS);  // full access 

                SERVICE_STATUS_PROCESS service_status_address;
                DWORD bytes_needed; //number of bytes needed to store all status information

                if (!QueryServiceStatusEx(
                    service_handle, // handle to service 
                    SC_STATUS_PROCESS_INFO, // info level
                    (LPBYTE)&service_status_address, // address of structure
                    sizeof(SERVICE_STATUS_PROCESS), // size of structure
                    &bytes_needed))              // if buffer too small
                {
                    printf("\nQueryServiceStatusEx FAILED %d:\n\n", GetLastError());
                    printf("The specified service does not exist as an installed service");
                    CloseServiceHandle(schSCManager);
                    return 1;
                }
                else
                {
                    cout << "\nSERVICE_NAME: " << service_name << "\n";
                    wcout << "\tTYPE\t\t\t: " << service_status_address.dwServiceType;
                    switch (service_status_address.dwServiceType) 
                    {
                    case SERVICE_FILE_SYSTEM_DRIVER:
                        wcout << "  FILE_SYSTEM_DRIVER\n";
                        break;
                    case SERVICE_KERNEL_DRIVER:
                        wcout << "  KERNEL_DRIVER\n";
                        break;
                    case SERVICE_WIN32_OWN_PROCESS:
                        wcout << "  WIN32_OWN_PROCESS\n";
                        break;
                    case SERVICE_WIN32_SHARE_PROCESS:
                        wcout << "  WIN32_SHARE_PROCESS\n";
                        break;
                    case SERVICE_INTERACTIVE_PROCESS:
                        wcout << "  INTERACTIVE_PROCESS\n";
                        break;
                    case SERVICE_WIN32:
                        wcout << "  WIN32\n";
                        break;
                    default:
                        wcout << service_status_address.dwServiceType << "\n";
                        break;
                    };

                    wcout << "\tSTATE\t\t\t: " << service_status_address.dwCurrentState;

                    switch (service_status_address.dwCurrentState)
                    {
                    case SERVICE_RUNNING:
                        wcout << "  RUNNING\n";
                        break;
                    case SERVICE_STOPPED:
                        wcout << "  STOPPED\n";
                        break;
                    case SERVICE_CONTINUE_PENDING:
                        wcout << "  CONTINUING\n";
                        break;
                    case SERVICE_PAUSE_PENDING:
                        wcout << "  PAUSING\n";
                        break;
                    case SERVICE_PAUSED:
                        wcout << "  PAUSED\n";
                        break;
                    case SERVICE_START_PENDING:
                        wcout << "  STARTING\n";
                        break;
                    case SERVICE_STOP_PENDING:
                        wcout << "  STOPPING\n";
                        break;
                    default:
                        wcout << service_status_address.dwCurrentState << "\n";
                        break;
                    };

                    wcout << " \t\t\t\t ";
                    switch (service_status_address.dwControlsAccepted)
                    {
                    case SERVICE_ACCEPT_NETBINDCHANGE:
                        wcout << "(ACCEPT_NETBINDCHANGE)\n";
                        break;
                    case SERVICE_ACCEPT_PARAMCHANGE:
                        wcout << "(ACCEPT_PARAMCHANGE)\n";
                        break;
                    case SERVICE_ACCEPT_PAUSE_CONTINUE:
                        wcout << "(ACCEPT_PAUSE_CONTINUE)\n";
                        break;
                    case SERVICE_ACCEPT_PRESHUTDOWN:
                        wcout << "(ACCEPT_PRESHUTDOWN)\n";
                        break;
                    case SERVICE_ACCEPT_SHUTDOWN:
                        wcout << "(ACCEPT_SHUTDOWN)\n";
                        break;
                    case SERVICE_ACCEPT_STOP:
                        wcout << "(STOPPABLE)\n";
                        break;
                    default:
                        wcout << "(" << service_status_address.dwControlsAccepted << ")\n";
                        break;
                    };
                    wcout << "\tWIN32_EXIT_CODE\t\t:" << service_status_address.dwWin32ExitCode << "\n";
                    wcout << "\tSERVICE_EXIT_CODE\t:" << service_status_address.dwServiceSpecificExitCode << "\n";
                    wcout << "\tCHECKPOINT\t\t:" << service_status_address.dwCheckPoint << "\n";
                    wcout << "\tWAIT_HINT\t\t:" << service_status_address.dwWaitHint << "\n";
                    wcout << "\n";
                }

                CloseServiceHandle(schSCManager);
            }

        }

        //CREATE
        if (vm.count("create"))
        {
            //Open Service Control Manager
            schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS); //All possible access rights

            if (schSCManager == NULL) 
            {
                printf("OpenSCManager() failed with error %u\n", GetLastError());
                return 1;
            }

           // cout << "Test: create parameter was passed";          

            string help_create = "\nDESCRIPTION:\n \tCreates a service entry in the registry and Service Database. \nUSAGE :\n"
                                        "\tsc <server> create[service name][binPath=] <option1> <option2>...\n\nOPTIONS :\nNOTE : The option name includes the equal sign.\n"
                                        "\tA space is required between the equal sign and the value.\n"
                                        "type = <own | share | interact | kernel | filesys | rec | userown | usershare>\n\t(default = own)\n"
                                        "start = <boot | system | auto | demand | disabled | delayed - auto>\n \t(default = demand)\n"
                                        "error = <normal | severe | critical | ignore>\n\t(default = normal)\n"
                                        "binPath = <BinaryPathName to the.exe file>\n"
                                        "group = <LoadOrderGroup>\n"
                                        "tag = <yes | no>\n"
                                        "depend = <Dependencies(separated by / (forward slash))>\n"
                                        "obj = <AccountName | ObjectName>\n\t(default = LocalSystem)\n"
                                        "DisplayName = <display name>\n"
                                        "password = <password>\n";

            // Since Boost Program Options can't check for a specific value
            // Check if non-default values for required create service parameters have passed

            if (service_name == "")
            {
                cout << help_create;
                return 1;
            }
            else if (binary_path == "")
            {
                cout << help_create;
                return 1;
            }

            DWORD service_type;
            if (vm.count("type="))
            {
                if (vm["type="].as<string>() == "rec")
                {
                    service_type = SERVICE_RECOGNIZER_DRIVER;
                }
                else if (vm["type="].as<string>() == "own")
                {
                    service_type = (SERVICE_WIN32_OWN_PROCESS | SERVICE_USER_OWN_PROCESS);
                }
                else if (vm["type="].as<string>() == "share")
                {
                    service_type = (SERVICE_WIN32_SHARE_PROCESS | SERVICE_USER_SHARE_PROCESS);
                }
                else if (vm["type="].as<string>() == "kernel")
                {
                    service_type = SERVICE_KERNEL_DRIVER;
                }
                else if (vm["type="].as<string>() == "filesys")
                {
                    service_type = SERVICE_FILE_SYSTEM_DRIVER;
                }
                else if (vm["type="].as<string>() == "interact type= own" || vm["type="].as<string>() == "own type= share")
                {
                    service_type = (SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS);
                }
                else if (vm["type="].as<string>() == "interact type= share" || vm["type="].as<string>() == "share type= interact")
                {
                    service_type = (SERVICE_WIN32_SHARE_PROCESS | SERVICE_INTERACTIVE_PROCESS);
                }
                //Not doing the adapt switches, because it isn't defined in documentation
            }
            else
            {
                service_type = SERVICE_WIN32_OWN_PROCESS;
            }

            DWORD start_type;
            if (vm.count("start="))
            {
                if (vm["start="].as<string>() == "boot")
                {
                    start_type = SERVICE_BOOT_START;
                }
                else if (vm["start="].as<string>() == "system")
                {
                    start_type = SERVICE_SYSTEM_START;
                }
                else if (vm["start="].as<string>() == "auto")
                {
                    start_type = SERVICE_AUTO_START;
                }
                else if (vm["start="].as<string>() == "demand")
                {
                    start_type = SERVICE_DEMAND_START;
                }
                else if (vm["start="].as<string>() == "disabled")
                {
                    start_type = SERVICE_DISABLED;
                }
                else if (vm["start="].as<string>() == "delayed-auto")
                {
                    start_type = SERVICE_AUTO_START;
                }
            }
            else
            {
                start_type = SERVICE_DEMAND_START; //default value
            }

            DWORD error_control;
            if (vm.count("error="))
            {
                if (vm["error="].as<string>() == "normal")
                {
                    error_control = SERVICE_ERROR_NORMAL;
                }
                else if (vm["error="].as<string>() == "severe")
                {
                    error_control = SERVICE_ERROR_SEVERE;
                }
                else if (vm["error="].as<string>() == "critical")
                {
                    error_control = SERVICE_ERROR_CRITICAL;
                }
                else if (vm["error="].as<string>() == "ignore")
                {
                    error_control = SERVICE_ERROR_IGNORE;
                }
            }
            else
            {
                error_control = SERVICE_ERROR_NORMAL;  //default value
            }

            LPDWORD tag = nullptr;
            if (vm.count("tag="))
            {
                if (vm["tag="].as<string>() == "yes" || vm["tag="].as<string>() == "no")
                {
                    if (vm["start="].as<string>() == "system")
                    {
                        tag = vm["tag="].as<LPDWORD>();
                    }
                    else if (vm["start="].as<string>() == "boot")
                    {
                        tag = vm["tag="].as<LPDWORD>();
                    }
                }
                else
                {
                    tag = nullptr; //default value if incorrect value is passed to tag
                }
            }
            else
            {
                tag = nullptr; //default value - if you are not changing the existing tag.
            }
            
            
            //Convert parameter strings to const char* == LPCSTR
           // LPCSTR binary_path_input = binary_path;
            const char* service_name_input = service_name.c_str();
            LPCSTR binary_path_input = binary_path.c_str();

            //Assign lpServiceStartName if certain conditions met
            
            if (service_type == (SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS) ||
                service_type == (SERVICE_WIN32_SHARE_PROCESS | SERVICE_INTERACTIVE_PROCESS))
            {
                obj = "";
            }


            const char* loadordergrouph_input = "";
            if (loadordergroup != "")
            {
                const char* loadordergrouph_input;
                loadordergrouph_input = loadordergroup.c_str(); //lpLoadOrderGroup
            }

            const char* depend_input = "";
            if (depend != "")
            {
                const char* depend_input;
                depend_input = depend.c_str();
            }
            
            if (obj != "") 
            {
                const char* obj_input;
                obj_input = obj.c_str(); //lpServiceStartName
            }


            const char* display_name_input;
            display_name_input = service_name_input;//re-use the service name for the display name if no display name provided

            if (display_name != "")
            {
                display_name_input = display_name.c_str();
            }

            
            if (password != "")
            {
                const char* password_input;
                password_input = password.c_str();
            }
            
            const char* password_input;
            password_input = "";
           
            

            SC_HANDLE create_service;
            create_service = CreateServiceA(schSCManager, service_name_input,
                display_name_input, SC_MANAGER_ALL_ACCESS,
                service_type, start_type, error_control,
                binary_path_input, loadordergrouph_input,
                tag, depend_input, NULL,
                password_input);
             
            if (create_service != NULL)
            {
                printf("\nCreateService SUCCESS");
            }
            else 
            {
                printf("\nCreateService FAILED with error %u\n", GetLastError());
                CloseServiceHandle(schSCManager);
                return 0;
            }

            CloseServiceHandle(schSCManager);
        }

        // STOP
        if (vm.count("stop"))
        {
            //Open Service Control Manager
            schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS); //All possible access rights

            const char* service_name_input;
            service_name_input = service_name.c_str();

            SC_HANDLE service_handle;
            service_handle = OpenServiceA(schSCManager,         // SCM database 
                                            service_name_input, // name of service 
                                            SERVICE_ALL_ACCESS);

            SERVICE_STATUS service_status;
            

            if (ControlService(service_handle,
                                SERVICE_CONTROL_STOP,
                                &service_status))
            {
                wcout << "\nStopService SUCCESS" << "\n";
                CloseServiceHandle(schSCManager);
                CloseServiceHandle(service_handle);
                return 0;
            }
            else
            {
                wcout << "\nStopService FAILED : " << GetLastError() << "\n";
                CloseServiceHandle(schSCManager);
                CloseServiceHandle(service_handle);
                return 1;
            }
        }

        //DELETE
        if (vm.count("delete"))
        {
            //Open Service Control Manager
            schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS); //All possible access rights

            const char* service_name_input;
            service_name_input = service_name.c_str();

            SC_HANDLE service_handle;
            service_handle = OpenServiceA(schSCManager,         // SCM database 
                                            service_name_input, // name of service 
                                            SERVICE_ALL_ACCESS);

            if (DeleteService(service_handle)) 
            {
                wcout << "DeleteService SUCCESS";
                CloseServiceHandle(schSCManager);
                return 1;
            }
            else
            {
                wcout << "OpenService FAILED %d" << GetLastError();
                CloseServiceHandle(schSCManager);
                return 1;
            }
        }

        //CONFIG
        if(vm.count("config"))
        {
            //Open Service Control Manager
            schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS); //All possible access rights

            if (schSCManager != NULL)
            {
                printf("OpenSCManager() is OK!\n");
            }
            else
            {
                printf("OpenSCManager() failed with error %u\n", GetLastError());
                return 1;
            }

           // cout << "Test: config parameter was passed";

            string help_config = "\nDESCRIPTION :"
                                "\n\t\tModifies a service entry in the registry and Service Database."
                                "\nUSAGE :"
                                "\n\t\tsc <server> config[service name] <option1> <option2>..."
                                "\nOPTIONS :"
                                "\nNOTE : The option name includes the equal sign."
                                "\n\t\tA space is required between the equal sign and the value."
                                "\n\t\tTo remove the dependency, use a single / as dependency value."
                                "\ntype = <own | share | interact | kernel | filesys | rec | adapt | userown | usershare>"
                                "\nstart = <boot | system | auto | demand | disabled | delayed - auto>"
                                "\nerror = <normal | severe | critical | ignore>"
                                "\nbinPath = <BinaryPathName to the.exe file>"
                                "\ngroup = <LoadOrderGroup>"
                                "\ntag = <yes | no>"
                                "\ndepend = <Dependencies(separated by / (forward slash))>"
                                "\nobj = <AccountName | ObjectName>"
                                "\nDisplayName = <display name>"
                                "\npassword = <password>";

            // Since Boost Program Options can't check for a specific value
            // Check if non-default values for required create service parameters have passed
            if (service_name == "")
            {
                cout << help_config;
                return 1;
            }


            if (binary_path == "")
            {
                cout << help_config;
                return 1;
            }

            DWORD service_type;
            if (vm.count("type="))
            {
                if (vm["type="].as<string>() == "own")
                {
                    service_type = SERVICE_WIN32_OWN_PROCESS ;
                }
                else if (vm["type="].as<string>() == "share")
                {
                    service_type = SERVICE_WIN32_SHARE_PROCESS ;
                }
                else if (vm["type="].as<string>() == "kernel")
                {
                    service_type = SERVICE_KERNEL_DRIVER;
                }
                else if (vm["type="].as<string>() == "filesys")
                {
                    service_type = SERVICE_FILE_SYSTEM_DRIVER;
                }
                else if (vm["type="].as<string>() == "interact type=own" || vm["type="].as<string>() == "own type=share")
                {
                    service_type = SERVICE_INTERACTIVE_PROCESS;
                }
                else if (vm["type="].as<string>() == "interact type=share" || vm["type="].as<string>() == "share type=interact")
                {
                    service_type = (SERVICE_WIN32_SHARE_PROCESS | SERVICE_INTERACTIVE_PROCESS);
                }
                //Not doing the adapt switches, because it isn't defined in documentation
            }
            else
            {
                service_type = SERVICE_NO_CHANGE;
            }

            DWORD start_type;
            if (vm.count("start="))
            {
                if (vm["start="].as<string>() == "boot")
                {
                    start_type = SERVICE_BOOT_START;
                }
                else if (vm["start="].as<string>() == "system")
                {
                    start_type = SERVICE_SYSTEM_START;
                }
                else if (vm["start="].as<string>() == "auto")
                {
                    start_type = SERVICE_AUTO_START;
                }
                else if (vm["start="].as<string>() == "demand")
                {
                    start_type = SERVICE_DEMAND_START;
                }
                else if (vm["start="].as<string>() == "disabled")
                {
                    start_type = SERVICE_DISABLED;
                }
                else if (vm["start="].as<string>() == "delayed-auto")
                {
                    start_type = SERVICE_AUTO_START;
                }
            }
            else
            {
                start_type = SERVICE_NO_CHANGE; //default value
            }

            DWORD error_control;
            if (vm.count("error="))
            {
                if (vm["error="].as<string>() == "normal")
                {
                    error_control = SERVICE_ERROR_NORMAL;
                }
                else if (vm["error="].as<string>() == "severe")
                {
                    error_control = SERVICE_ERROR_SEVERE;
                }
                else if (vm["error="].as<string>() == "critical")
                {
                    error_control = SERVICE_ERROR_CRITICAL;
                }
                else if (vm["error="].as<string>() == "ignore")
                {
                    error_control = SERVICE_ERROR_IGNORE;
                }
            }
            else
            {
                error_control = SERVICE_ERROR_NORMAL;  //default value
            }


            LPDWORD tag = nullptr;  //default value - if you are not changing the existing tag.
            if (vm.count("tag="))
            {
                if (vm["tag="].as<string>() == "yes" || vm["tag="].as<string>() == "no")
                {
                    if (vm["start="].as<string>() == "system")
                    {
                        tag = vm["tag="].as<LPDWORD>();
                    }
                    else if (vm["start="].as<string>() == "boot")
                    {
                        tag = vm["tag="].as<LPDWORD>();
                    }
                }
            }
            

            //Convert parameter strings to const char* == LPCSTR
            const char* service_name_input = service_name.c_str();
            LPCSTR binary_path_input = binary_path.c_str();

            //Assign lpServiceStartName if certain conditions met

            if (service_type == (SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS) ||
                service_type == (SERVICE_WIN32_SHARE_PROCESS | SERVICE_INTERACTIVE_PROCESS))
            {
                obj = "";
            }


            const char* loadordergrouph_input = "";
            if (loadordergroup != "")
            {
                const char* loadordergrouph_input;
                loadordergrouph_input = loadordergroup.c_str(); //lpLoadOrderGroup
            }

            const char* depend_input = "";
            if (depend != "")
            {
                const char* depend_input;
                depend_input = depend.c_str();
            }


            if (obj != "")
            {
                const char* obj_input;
                obj_input = obj.c_str(); //lpServiceStartName
            }

            const char* display_name_input;
            display_name_input = service_name_input;//re-use the service name if no display name provided

            if (display_name != "")
            {
                display_name_input = display_name.c_str();
            }

            const char* password_input = "";

            if (password != "")
            {
                const char* password_input;
                password_input = password.c_str();
            }         

            SC_HANDLE service_handle;
            service_handle = OpenServiceA(schSCManager,  // SCM database 
                                            service_name_input, // name of service 
                                            SERVICE_CHANGE_CONFIG);

            if (ChangeServiceConfigA(service_handle,
                service_type, start_type, error_control,
                binary_path_input, loadordergrouph_input,
                tag, depend_input, NULL,
                password_input, display_name_input))
            {
                printf("\nChangeServiceConfigA SUCCESS");
                CloseServiceHandle(schSCManager);
                return 0;
            }
            else
            {
                printf("\nChangeServiceConfigA FAILED with error %u\n", GetLastError());
                CloseServiceHandle(schSCManager);
                return 1;
            }
        }

        //FAILURE
        if(vm.count("failure"))
        {
           // cout << "Test: failure parameter was passed\n";
            //Open Service Control Manager
            schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS); //All possible access rights

            if (schSCManager == NULL)
            {
                printf("OpenSCManager() failed with error %u\n", GetLastError());
                return 1;
            }

            string fail_message = "DESCRIPTION:\n\t\tChanges the actions upon failure\n"
                                        "USAGE :\n\t\t\tsc <server> failure[service name] <option1> <option2>..."
                                        "\nOPTIONS :\n"
                                        "\t\treset = <Length of period of no failures(in seconds)\n"
                                        "\t\t\tafter which to reset the failure count to 0 (may be INFINITE)>\n"
                                        "\n\t\t\t(Must be used in conjunction with actions = )"
                                        "\n\t\treboot = <Message broadcast before rebooting on failure>"
                                        "\n\t\tcommand = <Command line to be run on failure>"
                                        "\n\t\tactions = <Failure actions and their delay time(in milliseconds),\n" 
                                        "\t\t\tseparated by / (forward slash) --e.g., run / 5000 | reboot / 800"
                                        "\t\t\tValid actions are <run | restart | reboot> >"
                                        "\t\t\t(Must be used in conjunction with the reset = option)";
            
            // Since Boost Program Options can't check for a specific value
            // Check if non-default values for required create service parameters have passed
            if (service_name == "")
            {
                cout << fail_message;
                return 1;
            }

            vector<SC_ACTION> action_type;
            DWORD count_actions;
            
            //compile action vectors, which are converted to arrays
            if (vm["actions="].as<string>() != "") 
            {
                // separate multiple actions and their delay times
                
                vector<string> tokens;

                stringstream check1(actions);
                string item;

                // Tokenizing w.r.t. "|"
                while (getline(check1, item, '|'))
                {
                    tokens.push_back(item);
                }

                // Get action and time in correct format
                for (int i = 0; i < tokens.size(); i++)
                {
                     if (tokens[i].find("restart") != string::npos)
                     {  
                        //get time entered after action and convert to DWORD
                        string time = tokens[i].substr(tokens[i].find("/")+1);
                        DWORD time_input = strtoul(time.c_str(), NULL, 16);

                        action_type.push_back({ SC_ACTION_RESTART , time_input});
                     }
                     else if (tokens[i].find("reboot") != string::npos)
                     {
                         string time = tokens[i].substr(tokens[i].find("/") + 1);
                         DWORD time_input = strtoul(time.c_str(), NULL, 16);

                         action_type.push_back({ SC_ACTION_REBOOT , time_input });
                     }
                     else if (tokens[i].find("run") != string::npos)
                     {
                         string time = tokens[i].substr(tokens[i].find("/") + 1);
                         DWORD time_input = strtoul(time.c_str(), NULL, 16);

                         action_type.push_back({SC_ACTION_RUN_COMMAND, time_input });
                     }
                }
                 count_actions = (DWORD) tokens.size();
            }
            else
            {
                action_type.push_back({ SC_ACTION_NONE, 0 });
                count_actions = 1;
            }
            
            /* For some stupid reason, nested SC Actions must be passed as an array, not a vector
               to ChangeServiceConfig2 CAN NOT pass the count of actions as a const int to 
               initialize array, so pass 5, which should be big enough to handle all possible 
               actions. Unused spaces in array  don't cause errors 
            */

            SC_ACTION action_input[5];
            copy(begin(action_type), end(action_type), action_input);
             
            //convert reboot and command to LPSTR
            LPSTR reboot_input = const_cast<char*>(reboot.c_str());
            LPSTR command_input = const_cast<char*>(command.c_str());

            //convert reset to DWORD
            DWORD reset_input = static_cast<DWORD>(vm["reset="].as<int>());
            
            //Convert parameter strings to const char* == LPCSTR
            const char* service_name_input = service_name.c_str();

            SC_HANDLE service_handle;
            service_handle = OpenServiceA(schSCManager,  // SCM database 
                                        service_name_input, // name of service 
                                        SC_MANAGER_ALL_ACCESS);

            _SERVICE_FAILURE_ACTIONSA fail_action = { reset_input,
                                                    reboot_input, command_input, 
                                                    count_actions, action_input};
            
            if(ChangeServiceConfig2A(service_handle, SERVICE_CONFIG_FAILURE_ACTIONS, &fail_action))
            { 
                cout << "FAILURECONFIG SUCCESS";
            }
            else
            {
                cout << "FAILURECONFIG FAIL with error: %d" << GetLastError();
            }
        }

        //QDESCRIPTION
        if(vm.count("qdescription"))
        {
            schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE); //All possible access rights

            if (schSCManager == NULL)
            {
                printf("OpenSCManager() failed with error %u\n", GetLastError());
                return 1;
            }

           // cout << "Test: qdescription parameter was passed";

            // Check if non-default values for required qdescription switch have been passed
            if (service_name == "")
            {
                cout << "DESCRIPTION:\n\t\tRetrieves the description string of a service.\n";
                cout << "USAGE:\n\t\tsc <server> qdescription[service name] < bufferSize>\n";
                return 1;
            }

            // buffersize parameter
            DWORD buffsize;
            if (vm.count("bufsize="))
            {
                buffsize = vm["bufsize="].as<DWORD>();
            }
            else
            {
                buffsize = 1024; // default value
            }

            //Convert parameter strings to const char* == LPCSTR
            const char* service_name_input = service_name.c_str();

            SC_HANDLE service_handle;
            service_handle = OpenServiceA(schSCManager,  // SCM database 
                                          service_name_input, // name of service 
                                          SC_MANAGER_ALL_ACCESS);

            LPSERVICE_DESCRIPTION lpsd;
            DWORD dwBytesNeeded, cbBufSize;

            // Get needed buffer space
            if (!QueryServiceConfig2(service_handle,
                                    SERVICE_CONFIG_DESCRIPTION,
                                    NULL,
                                    buffsize,
                                    &dwBytesNeeded) && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
              
                cbBufSize = dwBytesNeeded;
                lpsd = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, cbBufSize);
            }
            else
            {
                printf("QueryServiceConfig2 failed (%d)", GetLastError());
                return 1;
            }

            if (!QueryServiceConfig2(service_handle,
                                    SERVICE_CONFIG_DESCRIPTION,
                                    (LPBYTE) lpsd,
                                    cbBufSize,
                                    &dwBytesNeeded))
            {
                printf("QueryServiceConfig2 FAILED %d:\n\n", GetLastError());
                printf("The specified service does not exist as an installed service");
                CloseServiceHandle(schSCManager);
                return 1;
            }
            else
            {
                cout << "QueryServiceConfig2 SUCCESS\n";
                cout << "\nSERVICE_NAME: " << service_name << "\n";
                wcout << "DESCRIPTION:  " << lpsd->lpDescription << "\n";
            }
        }

        //START
        if(vm.count("start"))
        {
            schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS); //All possible access rights

            if (schSCManager == NULL)
            {
                printf("OpenSCManager() failed with error %u\n", GetLastError());
                return 1;
            }

           // cout << "Test: start parameter was passed";

            // Check if non-default values for required create service parameters have passed
            if (service_name == "")
            {
                cout << "DESCRIPTION:\n\t\tStarts a service running.";
                cout << "USAGE:\n\t\tsc <server> start[service name] <arg1> <arg2> ...\n";
                return 1;
            }

            //Convert parameter strings to const char* == LPCSTR
            const char* service_name_input = service_name.c_str();

            SC_HANDLE service_handle;
            service_handle = OpenServiceA(schSCManager,  // SCM database 
                                         service_name_input, // name of service 
                                         SERVICE_ALL_ACCESS);

            SERVICE_STATUS_PROCESS service_status;
            DWORD dwOldCheckPoint;
            DWORD dwStartTickCount;
            DWORD dwWaitTime;
            DWORD dwBytesNeeded;

            // Check the status in case the service is not stopped. 

            if (!QueryServiceStatusEx(
                                        service_handle, // handle to service 
                                        SC_STATUS_PROCESS_INFO, // information level
                                        (LPBYTE)&service_status,  // address of structure
                                        sizeof(SERVICE_STATUS_PROCESS), // size of structure
                                        &dwBytesNeeded)) // size needed if buffer is too small
            {
                printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
                CloseServiceHandle(service_handle);
                CloseServiceHandle(schSCManager);
                return 1;
            }

            // Check if the service is already running. It would be possible 
            // to stop the service here, but for simplicity this example just returns and error int. 

            if (service_status.dwCurrentState != SERVICE_STOPPED && service_status.dwCurrentState != SERVICE_STOP_PENDING)
            {
                printf("Cannot start the service because it is already running\n");
                CloseServiceHandle(service_handle);
                CloseServiceHandle(schSCManager);
                return 1;
            }

            // Save the tick count and initial checkpoint.

            dwStartTickCount = GetTickCount64();
            dwOldCheckPoint = service_status.dwCheckPoint;

            // Wait for the service to stop before attempting to start it.

            while (service_status.dwCurrentState == SERVICE_STOP_PENDING)
            {
                // Do not wait longer than the wait hint. A good interval is 
                // one-tenth of the wait hint but not less than 1 second  
                // and not more than 10 seconds. 

                dwWaitTime = service_status.dwWaitHint / 10;

                if (dwWaitTime < 1000)
                    dwWaitTime = 1000;
                else if (dwWaitTime > 10000)
                    dwWaitTime = 10000;

                Sleep(dwWaitTime);

                // Check the status until the service is no longer stop pending. 

                if (!QueryServiceStatusEx(service_handle, // handle to service 
                                          SC_STATUS_PROCESS_INFO, // information level
                                          (LPBYTE)&service_status, // address of structure
                                          sizeof(SERVICE_STATUS_PROCESS), // size of structure
                                          &dwBytesNeeded)) // size needed if buffer is too small
                {
                    printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
                    CloseServiceHandle(service_handle);
                    CloseServiceHandle(schSCManager);
                    return 1;
                }

                if (service_status.dwCheckPoint > dwOldCheckPoint)
                {
                    // Continue to wait and check.
                    dwStartTickCount = GetTickCount64();
                    dwOldCheckPoint = service_status.dwCheckPoint;
                }
                else
                {
                    if (GetTickCount64() - dwStartTickCount > service_status.dwWaitHint)
                    {
                        printf("Timeout waiting for service to stop\n");
                        CloseServiceHandle(service_handle);
                        CloseServiceHandle(schSCManager);
                        return 1;
                    }
                }
            }

            // Attempt to start the service.
            if (!StartServiceA(service_handle, 0, NULL))
            {
                printf("\nStartService FAILED (%d)\n", GetLastError());
                CloseServiceHandle(service_handle);
                CloseServiceHandle(schSCManager);
                return 1;
            }
            else printf("\nService start pending...\n");

            // Check the status until the service is no longer start pending. 

            if (!QueryServiceStatusEx( service_handle, // handle to service 
                                        SC_STATUS_PROCESS_INFO, // info level
                                        (LPBYTE)&service_status, // address of structure
                                        sizeof(SERVICE_STATUS_PROCESS), // size of structure
                                        &dwBytesNeeded)) // if buffer too small
            {
                printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
                CloseServiceHandle(service_handle);
                CloseServiceHandle(schSCManager);
                return 1;
            }

            // Save the tick count and initial checkpoint
            dwStartTickCount = GetTickCount64();
            dwOldCheckPoint = service_status.dwCheckPoint;

            while (service_status.dwCurrentState == SERVICE_START_PENDING)
            {
                // Do not wait longer than the wait hint. A good interval is 
                // one-tenth the wait hint, but no less than 1 second and no 
                // more than 10 seconds. 

                dwWaitTime = service_status.dwWaitHint / 10;

                if (dwWaitTime < 1000)
                    dwWaitTime = 1000;
                else if (dwWaitTime > 10000)
                    dwWaitTime = 10000;

                Sleep(dwWaitTime);

                // Check the status again. 

                if (!QueryServiceStatusEx(service_handle,             // handle to service 
                                        SC_STATUS_PROCESS_INFO, // info level
                                        (LPBYTE)&service_status,             // address of structure
                                        sizeof(SERVICE_STATUS_PROCESS), // size of structure
                                        &dwBytesNeeded))              // if buffer too small
                {
                    printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
                    break;
                }

                if (service_status.dwCheckPoint > dwOldCheckPoint)
                {
                    // Continue to wait and check.

                    dwStartTickCount = GetTickCount64();
                    dwOldCheckPoint = service_status.dwCheckPoint;
                }
                else
                {
                    if (GetTickCount64() - dwStartTickCount > service_status.dwWaitHint)
                    {
                        // No progress made within the wait hint.
                        break;
                    }
                }
            }

            // Determine whether the service is running.

            if (service_status.dwCurrentState == SERVICE_RUNNING)
            {
                cout << "\nService started successfully.\n";
                cout << "\nSERVICE_NAME: " << service_name << "\n";
                wcout << "\tTYPE\t\t\t: " << service_status.dwServiceType;
                switch (service_status.dwServiceType)
                {
                case SERVICE_FILE_SYSTEM_DRIVER:
                    wcout << "  FILE_SYSTEM_DRIVER\n";
                    break;
                case SERVICE_KERNEL_DRIVER:
                    wcout << "  KERNEL_DRIVER\n";
                    break;
                case SERVICE_WIN32_OWN_PROCESS:
                    wcout << "  WIN32_OWN_PROCESS\n";
                    break;
                case SERVICE_WIN32_SHARE_PROCESS:
                    wcout << "  WIN32_SHARE_PROCESS\n";
                    break;
                case SERVICE_INTERACTIVE_PROCESS:
                    wcout << "  INTERACTIVE_PROCESS\n";
                    break;
                case SERVICE_WIN32:
                    wcout << "  WIN32\n";
                    break;
                default:
                    wcout << service_status.dwServiceType << "\n";
                    break;
                };

                wcout << "\tSTATE\t\t\t: " << service_status.dwCurrentState;

                switch (service_status.dwCurrentState)
                {
                case SERVICE_RUNNING:
                    wcout << "  RUNNING\n";
                    break;
                case SERVICE_STOPPED:
                    wcout << "  STOPPED\n";
                    break;
                case SERVICE_CONTINUE_PENDING:
                    wcout << "  CONTINUING\n";
                    break;
                case SERVICE_PAUSE_PENDING:
                    wcout << "  PAUSING\n";
                    break;
                case SERVICE_PAUSED:
                    wcout << "  PAUSED\n";
                    break;
                case SERVICE_START_PENDING:
                    wcout << "  STARTING\n";
                    break;
                case SERVICE_STOP_PENDING:
                    wcout << "  STOPPING\n";
                    break;
                default:
                    wcout << service_status.dwCurrentState << "\n";
                    break;
                };

                wcout << " \t\t\t\t ";
                switch (service_status.dwControlsAccepted)
                {
                case SERVICE_ACCEPT_NETBINDCHANGE:
                    wcout << "(ACCEPT_NETBINDCHANGE)\n";
                    break;
                case SERVICE_ACCEPT_PARAMCHANGE:
                    wcout << "(ACCEPT_PARAMCHANGE)\n";
                    break;
                case SERVICE_ACCEPT_PAUSE_CONTINUE:
                    wcout << "(ACCEPT_PAUSE_CONTINUE)\n";
                    break;
                case SERVICE_ACCEPT_PRESHUTDOWN:
                    wcout << "(ACCEPT_PRESHUTDOWN)\n";
                    break;
                case SERVICE_ACCEPT_SHUTDOWN:
                    wcout << "(ACCEPT_SHUTDOWN)\n";
                    break;
                case SERVICE_ACCEPT_STOP:
                    wcout << "(STOPPABLE)\n";
                    break;
                default:
                    wcout << "(" << service_status.dwControlsAccepted << ")\n";
                    break;
                };
                wcout << "\tWIN32_EXIT_CODE\t\t:" << service_status.dwWin32ExitCode << "\n";
                wcout << "\tSERVICE_EXIT_CODE\t:" << service_status.dwServiceSpecificExitCode << "\n";
                wcout << "\tCHECKPOINT\t\t:" << service_status.dwCheckPoint << "\n";
                wcout << "\tWAIT_HINT\t\t:" << service_status.dwWaitHint << "\n";
                wcout << "\n";
            }
            else
            {
                printf("Service not started. \n");
                printf("  Current State: %d\n", service_status.dwCurrentState);
                printf("  Exit Code: %d\n", service_status.dwWin32ExitCode);
                printf("  Check Point: %d\n", service_status.dwCheckPoint);
                printf("  Wait Hint: %d\n", service_status.dwWaitHint);
            }

            CloseServiceHandle(service_handle);
            CloseServiceHandle(schSCManager);
        }

    }
    catch (const boost::program_options::error &ex)
    {
        cerr << ex.what() << '\n';
    }
    return 0;
}







