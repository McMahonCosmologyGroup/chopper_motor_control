//*****************************************************************************
// $Archive: /ClearPath SC/Installer/Program Files/Teknic/ClearView/sdk/examples/4-Example-Motion/Example-Motion.cpp $
// $Revision: 3 $ $Date: 11/22/16 3:49p $
// $Workfile: Example-Motion.cpp $
//*****************************************************************************

//Required include files
#include <stdio.h>  
#include <string>
#include "pubSysCls.h"  
#include <iostream>

using namespace sFnd;


//*********************************************************************************
//This program will load configuration files onto each node connected to the port, then executes
//sequential repeated moves on each axis.
//*********************************************************************************

#define PORT_NUM            5   //The port's COM number (as seen in device manager)
#define ACC_LIM_RPM_PER_SEC 100
#define VEL_LIM_RPM         100
#define ACC_LIM_CPS2        2000
#define VEL_LIM_CPS         2000        
#define MOVE_DISTANCE_CNTS  600000
#define COUNT_ENCODER       1   //"Counts" per rotation
#define NUM_MOVES           1


int main(int argc, char* argv[])
{
    float Vhz;
    float Vc;
    float Ts;
    float Dc;

    //These represent to locations of any and all config files to be loaded into the motors.  Uncomment line 96 to load configs.
    std::string CONFIG_FILES[] = { "C:\\filepath\\Axis0.mtr",
                                   "C:\\filepath\\Axis1.mtr",
                                   "C:\\filepath\\Axis2.mtr",
                                   "C:\\filepath\\Axis3.mtr" };


    //Create the SysManager object.  This will object will coordinate actions among various ports,
    // and within nodes.  In this example we use this object to setup and open our port.
    SysManager* myMgr = SysManager::Instance();                          //Create System Manager myMgr

    printf("Hello World, I am SysManager\n");   //Print to console

    printf("\n I will now open port \t%i \n \n", PORT_NUM);

    //This will try to open the port.  If there is an error/exception during the port opening
    //the code will jump to the catch loop where detailed information regarding the error will be displayed,
    //otherwise the catch loop is skipped over
    try
    {
        myMgr->ComHubPort(0, PORT_NUM);  //define the first SC Hub port (port 0) to be associated with COM 
                                        //PORT_NUM  (as seen in device manager)

        myMgr->PortsOpen(1);             //Open the first port.
    }
    catch (mnErr theErr)    //This catch statement will intercept any error from the Class library
    {
        printf("Port Failed to open, shutting down\n");
        //This statement will print the address of the error, the error code (defined by the mnErr class), 
        //as well as the corresponding error message.
        printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);

        ::system("pause"); //pause so the user can see the error message; waits for user to press a key

        return 0;  //This terminates the main program
    }

    //Once the code gets past this point, it can be assumed that the Port has been opened without issue
    //Now we can get a reference to our port object which we will use to access the node objects

    IPort& myPort = myMgr->Ports(0);     //set the reference myPort to refer to port 0 

    //Here we ensure that the port has the proper number of nodes and loads the stored config files onto each node
    if (sizeof(CONFIG_FILES) / sizeof(CONFIG_FILES[0]) < myPort.NodeCount()) {

        printf("Wrong number of nodes detected\n");
        printf("I have  \t%i  Nodes \n \n", myPort.NodeCount());  //Print how many nodes the port has

        ::system("pause"); //pause so the user can see the error message; waits for user to press a key

        return -1;  //This terminates the main program
    }

    try {
        for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
            // Create a shortcut reference for a node
            INode& theNode = myPort.Nodes(iNode);

            theNode.EnableReq(false);               //Ensure Node is disabled before loading config file

            myMgr->Delay(200);


            //Uncomment To load saved config files before motion
            //theNode.Setup.ConfigLoad(CONFIG_FILES[iNode].c_str());

            printf("   Node[%d]: type=%d\n", int(iNode), theNode.Info.NodeType());
            printf("            userID: %s\n", theNode.Info.UserID.Value());
            printf("        FW version: %s\n", theNode.Info.FirmwareVersion.Value());
            printf("          Serial #: %d\n", theNode.Info.SerialNumber.Value());
            printf("             Model: %s\n", theNode.Info.Model.Value());

            //The following statements will attempt to enable the node.  First,
            // any shutdowns or NodeStops are cleared, finally the node in enabled
            theNode.Status.AlertsClear();                   //Clear Alerts on node 
            theNode.Motion.NodeStopClear(); //Clear Nodestops on Node               
            theNode.EnableReq(true);                    //Enable node 


            double timeout = myMgr->TimeStampMsec() + 3000;          //define a timeout in case the node is unable to enable
                                                            //This will loop checking on the Real time values of the node's Ready status
            while (!theNode.Motion.IsReady()) {
                if (myMgr->TimeStampMsec() > timeout) {
                    printf("Error (v1): Timed out waiting for Node \t%i to enable\n", iNode);
                    return -2;
                }
            }
            //At this point the node is enabled
            printf("Node \t%i enabled\n", iNode);
        }
    }
    catch (mnErr theErr)
    {
        printf("Failed to load config files n\n");
        //This statement will print the address of the error, the error code (defined by the mnErr class), 
        //as well as the corresponding error message.
        printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);

        ::system("pause"); //pause so the user can see the error message; waits for user to press a key

        return 0;  //This terminates the main program
    }


    //At this point we will execute 10 rev moves sequentially on each axis
    try {
        for (size_t i = 0; i < NUM_MOVES; i++)
        {
            for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
                // Create a shortcut reference for a node
                INode& theNode = myPort.Nodes(iNode);


                theNode.Motion.MoveWentDone();                      //Clear the rising edge Move done register

                //theNode.AccUnit(INode::RPM_PER_SEC);                //Set the units for Acceleration to RPM/SEC
                //theNode.VelUnit(INode::RPM);                        //Set the units for Velocity to RPM
                //theNode.Motion.AccLimit = ACC_LIM_RPM_PER_SEC;      //Set Acceleration Limit (RPM/Sec)
                //theNode.Motion.VelLimit = VEL_LIM_RPM;              //Set Velocity Limit (RPM)
                theNode.AccUnit(INode::COUNTS_PER_SEC2);
                theNode.VelUnit(INode::COUNTS_PER_SEC);
                std::cout << "Velocity [Hz]: ";
                std::cin >> Vhz;
                std::cout << "Time [s]: ";
                std::cin >> Ts;
                Vc = Vhz * 800;
                Dc = Vc * Ts;

                theNode.Motion.AccLimit = ACC_LIM_CPS2;
                //theNode.Motion.VelLimit = VEL_LIM_CPS;
                theNode.Motion.VelLimit = Vc;

                printf("Moving Node \t%i \n", iNode);
                //theNode.Motion.MovePosnStart(MOVE_DISTANCE_CNTS);           //Execute 10000 encoder count move
                theNode.Motion.MovePosnStart(Dc);
                //theNode.Motion.MoveVelStart(VEL_LIM_RPM);
                printf("the velocity is \t%f \n", theNode.Motion.VelCommanded);
                double timeout = myMgr->TimeStampMsec() + theNode.Motion.MovePosnDurationMsec(Dc) + 100;         //define a timeout in case the node is unable to enable


                while (!theNode.Motion.MoveWentDone()) {
                    if (myMgr->TimeStampMsec() > timeout) {
                        printf("Error (v2): Timed out waiting for move to complete\n");
                        return -2;
                    }
                }
                printf("Node \t%i Move Done\n", iNode);

            }
        }
    }
    catch (mnErr theErr)
    {
        printf("Error during moves n\n");
        //This statement will print the address of the error, the error code (defined by the mnErr class), 
        //as well as the corresponding error message.
        printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);

        ::system("pause"); //pause so the user can see the error message; waits for user to press a key

        return 0;  //This terminates the main program
    }

    //After moves have completed Disable node, and close ports
    printf("Disabling nodes, and closing port\n");
    //Disable Nodes
    try {
        for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
            // Create a shortcut reference for a node
            myPort.Nodes(iNode).Outs.EnableReq(false);
        }
    }
    catch (mnErr theErr)
    {
        printf("Failed to disable Nodes n\n");
        //This statement will print the address of the error, the error code (defined by the mnErr class), 
        //as well as the corresponding error message.
        printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);

        ::system("pause"); //pause so the user can see the error message; waits for user to press a key

        return 0;  //This terminates the main program
    }

    // Close down the ports
    myMgr->PortsClose();

    ::system("pause"); //pause so the user can see the program output; waits for user to press a key
    return 0;           //End program
}
