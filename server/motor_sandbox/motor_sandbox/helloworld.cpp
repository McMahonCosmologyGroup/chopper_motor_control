//*****************************************************************************
// $Archive: /ClearPath SC/Installer/Program Files/Teknic/ClearView/sdk/examples/1-HelloWorld/HelloWorld.cpp $
// $Revision: 6 $ $Date: 11/22/16 3:26p $
// $Workfile: HelloWorld.cpp $
//*****************************************************************************

//Required include files
#include <stdio.h>      
#include "pubSysCls.h"  

using namespace sFnd;

/*****************************************************************************
*   Description:    This program is the bare minimum to open a port and identify the number of nodes
*       on the port.
*
*
*       Return:     0 if the system was initialized properly.
*                   -1 if there was a problem accessing the port.
*****************************************************************************/
int _tmain(int argc, _TCHAR* argv[])
{
    int portnum = 5;                            //The port's COM number (as seen in device manager)

    //Create the SysManager object. This object will coordinate actions among various ports
    // and within nodes. In this example we use this object to setup and open our port.
    SysManager* myMgr = SysManager::Instance();	                          //Create System Manager myMgr

    printf("Hello World, I am SysManager\n");   //Print to console

    printf("\n I will now open port \t%i \n \n", portnum);

    //This will try to open the port. If there is an error/exception during the port opening,
    //the code will jump to the catch loop where detailed information regarding the error will be displayed;
    //otherwise the catch loop is skipped over
    try
    {
        myMgr->ComHubPort(0, portnum);   //define the first SC Hub port (port 0) to be associated 
                                        // with COM portnum (as seen in device manager)

        myMgr->PortsOpen(1);             //Open the port
    }
    catch (mnErr theErr) //This catch statement will intercept any error from the Class library
    {
        printf("Port Failed to open, Check to ensure correct Port number and that ClearView is not using the Port\n");
        //This statement will print the address of the error, the error code (defined by the mnErr class), 
        //as well as the corresponding error message.
        printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);

        ::system("pause"); //pause so the user can see the error message; waits for user to press a key

        return -1;  //This terminates the main program
    }

    IPort& myPort = myMgr->Ports(0);

    printf(" Port[%d]: state=%d, nodes=%d\n",
        myPort.NetNumber(), myPort.OpenState(), myPort.NodeCount());

    // Close down the ports
    myMgr->PortsClose();

    ::system("pause"); //pause so the user can see the program output; waits for user to press a key

    return 0;           //End program
}
//
