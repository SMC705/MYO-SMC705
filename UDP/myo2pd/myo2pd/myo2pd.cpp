// OUR PERSONAL HELLO-MYO


#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

// Myo library
#include <myo/myo.hpp>      // Header containing all the classes related to the Myo

// UDP libraries
#include <sys/socket.h>
#include <netinet/in.h>

// Number of audio sources
#define N = 7;

using namespace std;


// Define a class that inherits from the Myo::DeviceListener class. A DeviceListener type receives events from the Myo (e.g. if it's paired/unpaired, locked/unlocked, informations about gestures, etc).

class DataCollector : public myo::DeviceListener {
public:
    // Class constructor
    DataCollector() : onArm(false), isUnlocked(false), roll_vol(0), currentPose()
    {
    }
    
    // N.B.: the following functions are defined in DeviceListener.hpp without specifications (they're virtual). Here they are re-defined with specifications.
    
    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_vol = 0;
        onArm = false;
        isUnlocked = false;
    }
    
    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented as a unit quaternion.
    // This function takes the position values from the quaternion and assign new values to roll_w, pitch_w and yaw_w every time new data are provided by the Myo.
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        using std::max;
        using std::min;
        
        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        
        // Convert the floating point angles in radians to a clockwise scale from 0 to 126 with the zero angle in π.
        // Need to change the format of the angle? DO IT HERE!
        
        // We define an angle format for the volume control here.
        // It is continuous from 0 to 126.
        roll_vol = ((static_cast<int>(( -roll + (float)M_PI)/(M_PI * 2.0f) * 1000)) - 400) * 127/200;
        
        // If the angle is negative => send 0 angle (used as "volume mute" command).
        // If the angle is higher then our maximum 126 => send 126 angle (used as volume max limit).
        if ( roll_vol < 1 ) roll_vol = 1;
        else if ( roll_vol > 127 ) roll_vol = 127;
    }
    
    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    // This function govern the lock-behaviour of the Myo: if there is a pose the Myo stay unlocked. If there's no pose then the Myo stays unlocked for only a short period of time.
    // Note that the member 'Pose' have a number of types corresponding to different poses --> see the SDK
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;
        
        if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
            // Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
            // Myo becoming locked.
            // If the pose is 'unknown' or 'rest', then the Myo gets locked.
            myo->unlock(myo::Myo::unlockHold);
            
            // Notify the Myo that the pose has resulted in an action, in this case changing
            // the text on the screen. The Myo will vibrate through the function 'notifyUserAction()'.
            myo->notifyUserAction();
        } else {
            // Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
            // are being performed, but lock after inactivity.
            // NOTE: how to eventually change 'unlockTimed'
            myo->unlock(myo::Myo::unlockTimed);
        }
    }
    
    
    // onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
                   myo::WarmupState warmupState)
    {
        onArm = true;
        whichArm = arm;
    }
    
    // onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }
    
    // onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
    void onUnlock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = true;
    }
    
    // onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
    void onLock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = false;
    }
    
    // We define this function to print the current values that were updated by the on...() functions above.
    // Need to change the all the output format on screen? DO IT HERE!
    void print()
    {
        // Clear the current line
        cout << '\r';
        /*
         // Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
         std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
         << '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
         << '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';
         */
        
        if (onArm) {
            // Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.
            
            // Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
            // output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
            // that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
            string poseString = currentPose.toString();
            
            cout << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
        } else {
            // Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
            std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
        }
        
        cout << flush;
    }
    
    // These values are set by onArmSync() and onArmUnsync() above.
    bool onArm;
    myo::Arm whichArm;
    
    // This is set by onUnlocked() and onLocked() above.
    bool isUnlocked;
    
    // These values are set by onOrientationData() and onPose() above.
    float roll_vol;
    myo::Pose currentPose;
};


//================================================================================




int main() {
    
    // The following code might generate exceptions => we try to execute it and catch any resulting exception
    try {
        // Create an Hub. The Hub provides access to one or more Myos.
        myo::Hub hub("com.SMC705.AAU");
        
        cout << "Attempting to find a Myo..." << endl;
        
        // waitForMyo function:
        // If a Myo is already paired in Myo Connect, this will return that Myo immediately.
        // Otherwise it waits a period of time (timeout_ms in millisec) and if a Myo isn't founded, the function returns a null pointer.
        // Need to change the timeout for attempting a Myo? DO IT HERE!
        myo::Myo* myo = hub.waitForMyo(10000);
        
        // If it fails in finding a Myo, a null pointer is returned and an error message is printed:
        if (!myo) {
            throw runtime_error("Unable to find a Myo!");
        }
        
        // If a Myo is founded, a message is printed.
        cout << "Connected to a Myo armband" << endl << endl;
        
        // Construct a DataCollector object, so that we can add it to the Hub and get data from the Myo.
        DataCollector collector;
        
        // Add the DataCollector object to the Hub.
        // Note: the Hub:addListener() takes the address (&) of any object defined as myo::DeviceListener.
        hub.addListener(&collector);
        
        //==========================================================================
        // UDP CONNECTION TO LOCALHOST
        
        int clientSocket, portNum, nBytes;
        //        char buffer[] = "pose";
        struct sockaddr_in serverAddr;
        socklen_t addr_size;
        
        /*Create UDP socket*/
        clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
        
        /*Configure settings in address struct*/
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(7890);      // port
        serverAddr.sin_addr.s_addr = 16777343;  // local address
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
        
        /*Initialize size variable to be used later on*/
        addr_size = sizeof serverAddr;
        
        
        
        
        //==========================================================================
        // LOOP
        
        // Enter a main loop that print position, pose and state of the Myo for every iteration.
        while(1) {
            // The function hub.run(duration) runs the event loop for the specified duration (ms).
            // Need to change the output rate? DO IT HERE!
            hub.run(1000/20);
            
            // Print the output as described in print() function.
            collector.print();
            
            
            char * buffer = new char[12];
            // Initialize the array of char
            buffer[0] = '/';
            buffer[1] = '0';
            buffer[2] = '0';
            buffer[3] = '0';
            buffer[4] = ',';
            buffer[5] = 'i';
            buffer[6] = '0';
            buffer[7] = '0';
            buffer[8] = '0';
            buffer[9] = '0';
            buffer[10] = 1;
            buffer[11] = '\0';
            
            
            // Send UDP package
            string pose2send = collector.currentPose.toString();
            
            // volume control
            if ( pose2send.compare("fist") == 0 ) {
                buffer[1] = 'v';
                buffer[2] = 'c';
                buffer[10] = collector.roll_vol;
            }
            
            // source control
            if ( pose2send.compare("fingersSpread") == 0 ) {
                buffer[1] = 's';
                buffer[2] = 'c';
                buffer[10] = collector.roll_vol;
            }
            
            // preset control
            else if ( pose2send.compare("waveIn") == 0 ) {
                buffer[1] = 'p';
                buffer[2] = 'c';
            }
            // mute
            else if ( pose2send.compare("waveOut") == 0 ) {
                buffer[1] = 'm';
                buffer[2] = 'm';
            }
            
            else if ( pose2send.compare("unknown") == 0 || pose2send.compare("rest") == 0)
                continue;
            
            nBytes = strlen(buffer) + 1;
            cout <<  collector.roll_vol << endl;
            /*Send message to server*/
            sendto(clientSocket,buffer,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);

            delete[] buffer;
        }
        

    }

// If a standard exception occurred, we print out its message and exit.
catch (const std::exception& e) {
    cerr << "Error: " << e.what() << endl;
    cerr << "Press enter to continue";
    cin.ignore();
    return 1;
    }
}