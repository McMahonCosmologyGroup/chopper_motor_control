// $Archive: /ClearPath SC/Installer/Program Files/Teknic/ClearView/sdk/examples/3-Example-StatusAlerts/Example-StatusAlerts.cpp $
// $Revision: 10 $ $Date: 11/22/16 3:45p $
// $Workfile: Example-StatusAlerts.cpp $
//******************************************************************************

#include <stdio.h>
#include <string>
#include "pubSysCls.h"

using namespace sFnd;
using namespace std;

//*********************************************************************************
//This program will 
//*********************************************************************************

#define PORT_NUM            5   //The port's COM number (as seen in device manager)

int main(int argc, char* argv[])
{

    // The network manager
    SysManager* myMgr = SysManager::Instance();	


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

    //Now we will check for alerts on all nodes on the port
    try {

        IPort& myPort = myMgr->Ports(0);     //set the reference myPort to refer to port 0 

        char alertList[256];

        printf("Checking for Alerts: \n");

        for (unsigned iNode = 0; iNode < myPort.NodeCount(); iNode++) {
            // Get a reference to the node, to make accessing it easier
            INode& theNode = myPort.Nodes(iNode);

            // make sure our registers are up to date
            theNode.Status.RT.Refresh();
            theNode.Status.Alerts.Refresh();

            printf("---------\n");
            printf(" Checking node %i for Alerts:\n", iNode);

            // Check the status register's "AlertPresent" bit
            // The bit is set true if there are alerts in the alert register
            if (!theNode.Status.RT.Value().cpm.AlertPresent) {
                printf("   Node has no alerts!\n");
            }

            // get an alert register reference, check the alert register directly for alerts
            if (theNode.Status.Alerts.Value().isInAlert()) {
                // get a copy of the alert register bits and a text description of all bits set
                theNode.Status.Alerts.Value().StateStr(alertList, 256);
                printf("   Node has alerts! Alerts:\n%s\n", alertList);

                // can access specific alerts using the method below
                if (theNode.Status.Alerts.Value().cpm.Common.EStopped) {
                    printf("      Node is e-stopped: Clearing E-Stop\n");
                    theNode.Motion.NodeStopClear();
                }
                if (theNode.Status.Alerts.Value().cpm.TrackingShutdown) {
                    printf("      Node exceeded Tracking error limit\n");
                }
                // Check for more alerts and Clear Alerts
                theNode.Status.Alerts.Refresh();
                if (theNode.Status.Alerts.Value().isInAlert()) {
                    theNode.Status.Alerts.Value().StateStr(alertList, 256);
                    printf("      Node has non-estop alerts: %s\n", alertList);
                    printf("      Clearing non-serious alerts\n");
                    theNode.Status.AlertsClear();

                    // Are there still alerts?
                    theNode.Status.Alerts.Refresh();
                    if (theNode.Status.Alerts.Value().isInAlert()) {
                        theNode.Status.Alerts.Value().StateStr(alertList, 256);
                        printf("   Node has serious, non-clearing alerts: %s\n", alertList);
                    }
                    else {
                        printf("   Node %d: all alerts have been cleared\n", theNode.Info.Ex.Addr());
                    }
                }
                else {
                    printf("   Node %d: all alerts have been cleared\n", theNode.Info.Ex.Addr());
                }

            }
            printf("=========\n");
        }

        printf("\nShutting down network\n");

        // Close down the ports
        myMgr->PortsClose();

    }
    catch (mnErr theErr) {
        fprintf(stderr, "Caught error: addr=%d, err=0x%0x\nmsg=%s\n",
            theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
        printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n",
            theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
        ::system("pause");
        // keep the console output readable
        printf("\n");
        return(2);
    }
    catch (...) {
        fprintf(stderr, "Error generic caught\n");
        printf("Generic error caught\n");
        ::system("pause");
        // keep the console output readable
        printf("\n");
        return(3);
    }

    // Good-bye
    ::system("pause");
    // keep the console output readable
    printf("\n");
    return 0;
}
//                                                                             *
//******************************************************************************
