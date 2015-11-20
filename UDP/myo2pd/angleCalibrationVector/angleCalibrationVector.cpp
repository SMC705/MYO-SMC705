#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

// Myo library (contains all the classes related to the Myo
#include <myo/myo.hpp>

// UDP libraries
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

// Number of audio sources
const int nSrc = 3;
// Number of audio presets
const int nPrst = 4;

// UDP connection settings (set ipAddress to 127.0.0.1 for local communication, 10.42.0.1 for Devid computer communication)
const int bufferLength = 4;



// Define a class that inherits from the Myo::DeviceListener class. A DeviceListener type receives events from the Myo (e.g. if it's paired/unpaired, locked/unlocked, informations about gestures, etc).
class DataCollector : public myo::DeviceListener {
public:
    // Class constructor
    DataCollector() : onArm(false), isUnlocked(false), roll_tmp(0), roll_vol(0), roll_src(0), currentPose()
    {
    }
    
    // N.B.: the following functions are defined in DeviceListener.hpp without specifications (they're virtual). Here they are re-defined with specifications.
    
    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_tmp = 0;
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
        
        roll_tmp = static_cast<int>( ( roll * 180/M_PI) );   // the value of 200 ensures a positive value for the angle.
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
            
            cout << "Pose: " << poseString << "  |  " << "Rotation: ";
            
            if ( poseString.compare("fist") == 0 )
                cout << roll_vol;
            else if ( poseString.compare("fingersSpread") == 0 )
                cout << roll_src;
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
    float roll_tmp, roll_vol, roll_src;
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
        
        vector<int> angles;
        if (cin.get()) {
        while ( 1 ) {
                hub.run(1000/20);
            cout << collector.roll_tmp << endl;
                angles.push_back( collector.roll_tmp);
            if ( angles.size() == 100 ) break;
            }
        }
 
        for (int i=0; i<angles.size(); i++) {
            for (int j=i+1; j<angles.size(); j++) {
                if ( angles[i] > angles[j] ) {
                    int temp = angles[i];
                    angles[i] = angles[j];
                    angles[j] = temp;
                }
            }
        }
        
        // A threshold value is added from the minimum and removed from the maximum in order to reduce the range: some users may exagerate the rotation during calibration.
        myo->vibrate(myo::Myo::vibrationShort);
        cout << "The minimum angle is: " << angles.front() + 10 << endl;
        cout << "The maximum angle is: " << angles.back() - 10 << endl;
    
  }
    // If a standard exception occurred, we print out its message and exit.
    catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        cerr << "Press enter to continue";
        cin.ignore();
        return 1;
    }
}