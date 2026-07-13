#ifndef WRITEPOLICY_HPP
#define WRITEPOLICY_HPP


class WritePolicy
{
    public:
        virtual bool writeThroughOnHit() const = 0; // Pure virtual function to determine if the write policy is write-through
        virtual bool writeAllocateOnMiss() const = 0; // Pure virtual function to determine if the write policy is write-allocate
        virtual ~WritePolicy() = default; // Virtual destructor for proper cleanup of derived classes
};

class WriteBackNoAllocate: public WritePolicy
{
    public:
        bool writeThroughOnHit() const override { return false; } // Override to indicate that this policy is write-back on hit
        bool writeAllocateOnMiss() const override { return false; } // Override to indicate that this policy is write-allocate on miss
};

class WriteThroughNoAllocate : public WritePolicy
{
    public:
        bool writeThroughOnHit() const override { return true; } // Override to indicate that this policy is write-through on hit
        bool writeAllocateOnMiss() const override { return false; } // Override to indicate that this policy is write-allocate on miss
};

class WriteThroughAllocate : public WritePolicy
{
    public:
        bool writeThroughOnHit() const override { return true; } // Override to indicate that this policy is write-through on hit
        bool writeAllocateOnMiss() const override { return true; } // Override to indicate that this policy is write-allocate on miss
};

class WriteBackAllocate : public WritePolicy
{
    public:
        bool writeThroughOnHit() const override { return false; } // Override to indicate that this policy is write-back on hit
        bool writeAllocateOnMiss() const override { return true; } // Override to indicate that this policy is write-allocate on miss
};









#endif //WRITEPOLICY_HPP