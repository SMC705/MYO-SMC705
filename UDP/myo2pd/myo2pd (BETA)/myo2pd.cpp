#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

// Myo library (contains all the classes related to the Myo)
#include <myo/myo.hpp>

// UDP libraries
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;


// Minimum and maximum roll angle
const int minAngle = -128;
const int maxAngle = 5;
// Number of audio sources
const int srcNum = 6;
// Single source rotation range
const int srcRange = 30;
// Total source rotation range
const int srcTotRange = srcRange * srcNum;
// Source selection threshold
const int srcThr = 5;
// Number of audio presets
const int presetNum = 3;

// UDP connection settings (set ipAddress to 127.0.0.1 for local communication, 10.42.0.1 for Devid computer communication)
const int portNum = 7890;
const char ipAddress[] = "127.0.0.1";
const int bufferLength = 4;



// Define a class that inherits from the Myo::DeviceListener class. A DeviceListener type receives events from the Myo (e.g. if it's paired/unpaired, locked/unlocked, informations about gestures, etc).
class DataCollector : public myo::DeviceListener {
public:
    // Class constructor
    DataCollector() : onArm(false), isUnlocked(false), roll_vol(0), roll_src(0), currentPose()
    {
    }
    
    // N.B.: the following functions are defined in DeviceListener.hpp without specifications (they're virtual). Here they are re-defined with specifications.
    
    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_vol = 0;
        roll_src = 0;
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
        
        // Convert the floating point angles in radians to a clockwise scale from 1 to 127 (good for PureData) with the zero angle in π.
        // Need to change the format of the angle? DO IT HERE!
        
        // We define an angle format for the volume control here.
        // It is continuous from 1 to 127.
        roll_vol = static_cast<int>( ( ( roll * 180/M_PI) - minAngle ) * 127/fabs(maxAngle-minAngle) );
        
        // If the angle is 0 or negative => send 1 angle (used as "volume mute" command).
        // If the angle is higher then our maximum 127 => send 127 angle (used as volume max limit).
        if ( roll_vol < 1 ) roll_vol = 1;
        else if ( roll_vol > 127 ) roll_vol = 127;
        
        // We define an angle format for the source control here.
        // The range [0, π] is divided into a number of equal ranges, as the number of sources srcNum.
        roll_src = static_cast<int>( ( ( roll * 180/M_PI) - minAngle ) * srcTotRange/fabs(maxAngle-minAngle) );
        if ( roll_src < 1 ) roll_src = 1;
        else if ( roll_src > srcTotRange ) roll_src = srcTotRange;
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

            // I commented this out because we want only our feedback vibrations!!!
//            myo->notifyUserAction();
            
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
        
        if (onArm) {
            // Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.
            
            // Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
            // output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
            // that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
            string poseString = currentPose.toString();
            
            cout << "Pose: " << poseString << "  |  " << "Rotation: ";
            
            if ( poseString.compare("fist") == 0 )
                cout << roll_vol;
            else if ( poseString.compare("fingersSpread") == 0 )
                cout << roll_src;
            else if ( poseString.compare("waveIn") == 0 )
                cout << 1;
            else if ( poseString.compare("waveOut") == 0 )
                cout << 2;
        }
        
        else {
            // Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
            cout << "[?]";
        }
        
        cout << flush;
    }
    
    // These values are set by onArmSync() and onArmUnsync() above.
    bool onArm;
    myo::Arm whichArm;
    
    // This is set by onUnlocked() and onLocked() above.
    bool isUnlocked;
    
    // These values are set by onOrientationData() and onPose() above.
    float roll_vol, roll_src;
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
        // UDP CONNECTION SETTINGS
        // need to change port number (portNum) or ip-address(ipAddress)? DO IT HERE!
        
        int clientSocket, nBytes;
        struct sockaddr_in serverAddr;
        socklen_t addr_size;
        
        /*Create UDP socket*/
        clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
        
        /*Configure settings in address struct*/
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(portNum);      // port
        serverAddr.sin_addr.s_addr = inet_addr(ipAddress);  // local address (16777343)
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
        
        /*Initialize size variable to be used later on*/
        addr_size = sizeof serverAddr;
        
        
        
        //==========================================================================
        // OUTPUT FILE SETTINGS
        // here we export all the data of a user to a file.
        
        ofstream outputFile;
        outputFile.open( "prova.dat");
        outputFile << "ciao\n";
        outputFile.close();
        
        
        
        //==========================================================================
        // WHILE LOOP
        
        char * buffer = new char[bufferLength];     // NOTE: allocate memory here or inside the loop????
        
        // Variables for haptic feedback.
        int samePoseTh = 15;    // if the same position is obtained for more then samePoseTh measures, then a vibration is sent.
        int samePoseTh2 = 5;    // this threshold is used to avoid noise in recognizing a command (fx: a fst can be recognized erroneously.
        int samePose = 0;       // same position index: keep trace of how many consecutive same position are measured.
        string previousPoseString = "blabla";   // previous position to be compared with each measured position (initialized "randomly" to blabla).
        
        int sameVolTh = 15;
        int sameVol = 0;

        // Variables for source selection
        int src = 0;
        
        // Variables for presets.
        int preset = 0;         // Current preset (initialized to zero)
        
        // Enter a main loop that print position, pose and state of the Myo for every iteration.
        while(1) {
            // The function hub.run(duration) runs the event loop for the specified duration (ms).
            // Need to change the output rate? DO IT HERE!
            hub.run(1000/20);
            
            bool hamlet = false;    // "to send or not to send" variable: if true => UDP message sent.
            
            string poseString = collector.currentPose.toString();
            
            
            // If measured pose is the same as previous => increment samePose.
            // If samePose index reaches the threshold for waveIn or waveOut => give haptic feedback.
            if ( poseString == previousPoseString )
                samePose++;
            else samePose = 0;
            

            
            // LOCK / UNLOCK
            // ----------------

/*
 
 NOTE: impossible to setup the isUnlock method in this way: the buffer will be surely overwritten by other gestures in the same iteration of the while loop.
 It's much better to implement another buffer with the message is unlocked --->>> TALK WITH DEVID WHICH WAY HE PREFERS TO RECEIVE IT!
            if ( collector.isUnlocked == true ) {
                buffer[0] = 'u';
                buffer[1] = 'l';
                buffer[2] = 'k';
                buffer[3] = 1;
            }

            else if ( collector.isUnlocked == false ) {
                buffer[0] = 'u';
                buffer[1] = 'l';
                buffer[2] = 'k';
                buffer[3] = 0;
            }
*/
            
            
            // VOLUME CONTROL
            // ----------------
            
            if ( poseString.compare("fist") == 0 && samePose > samePoseTh2 ) {
                buffer[0] = 'v';
                buffer[1] = 'o';
                buffer[2] = 'l';
                buffer[3] = collector.roll_vol;
                hamlet = true;
                
                // Haptic feedback on min and max volume
                if ( collector.roll_vol == 1 || collector.roll_vol == 127 ) {
                    sameVol++;
                    if ( sameVol == 1 ) {
                        myo->vibrate(myo::Myo::vibrationShort);
                    }
                    else if ( sameVol == sameVolTh ) {
                        sameVol = 0;
                    }
                }
                
                else sameVol = 0;
                
            }
   

            
            // PRESET SELECTION
            // ----------------

            // Presets increment
            else if ( poseString == "waveOut" && samePose == samePoseTh ) {
                preset++;
                if ( preset > presetNum ) preset = 0;
                
                // Fill buffer array with the preset number.
                buffer[0] = 'p';
                buffer[1] = 's';
                buffer[2] = 't';
                buffer[3] = preset;
                hamlet = true;
                
                // Haptic feedback and samePose reset
                myo->vibrate(myo::Myo::vibrationShort);
                samePose = 0;
            }
            
            // Presets decrement
            else if ( poseString == "waveIn" && samePose == samePoseTh ) {
                preset--;
                if ( preset < 0 ) preset = presetNum;
                
                // Fill buffer array with the preset number.
                buffer[0] = 'p';
                buffer[1] = 's';
                buffer[2] = 't';
                buffer[3] = preset;
                hamlet = true;
                
                // Haptic feedback and samePose reset
                myo->vibrate(myo::Myo::vibrationShort);
                samePose = 0;
            }
            
            

            // SOURCE SELECTION
            // ----------------
            
            // NOTE: if I scroll more than one source, the myo vibrates many times as the number of sources passed with the movement.
            
            else if ( poseString.compare("fingersSpread") == 0 && samePose > samePoseTh2 ) {
                
                int previousSrc = 0;
                int sameSrc = 0;
                int sameSrcTh = 5;
                if ( src == previousSrc ) {
                    sameSrc++;
                    cout << sameSrc << endl;
                    if ( sameSrc > sameSrcTh ) myo->vibrate(myo::Myo::vibrationShort);
                }
                
                // Source increment
                if ( collector.roll_src > src * srcRange + srcThr ) {
                    src++;
//                    myo->notifyUserAction();
                }
                else if ( collector.roll_src < (src-1) * srcRange - srcThr) {
                    src--;
//                    myo->vibrate(myo::Myo::vibrationShort);
                }
                
                previousSrc = src;
                
                buffer[0] = 's';
                buffer[1] = 'r';
                buffer[2] = 'c';
                buffer[3] = src;
                hamlet = true;
                cout << "\n" << src << endl;
            }
            
            previousPoseString = poseString;
            
            
            // PRINT OUTPUT
            // ----------------
            
            collector.print();
            
            
            
            // SEND UDP MESSAGE TO PURE DATA
            // -----------------------------
            
            // It sends messages only if hamlet is true ("to send or not to send?")
            if ( hamlet == true) {
                nBytes = bufferLength;
                sendto(clientSocket,buffer,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);
            }

        }
        
        delete[] buffer;
    }

// If a standard exception occurred, we print out its message and exit.
catch (const std::exception& e) {
    cerr << "Error: " << e.what() << endl;
    cerr << "Press enter to continue";
    cin.ignore();
    return 1;
    }
}