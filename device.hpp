#ifndef DEVICE_HPP
#define DEVICE_HPP

class Device {

protected:
    // Protected members can be accessed by derived classes
    int ticks_needed = 1; // Number of ticks needed for an operation(default is 1)
    int ticks_progress = 0; // Current progress in ticks (like the clock one but for this device and this operation)

public:
   
    virtual ~Device() = default;

    virtual void tick(); // Function to be called every clock tick

    virtual void execute_operation() = 0; // Pure virtual function to be implemented by derived classes


    
};

#endif // DEVICE_HPP