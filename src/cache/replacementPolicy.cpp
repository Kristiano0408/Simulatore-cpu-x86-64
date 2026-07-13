#include "cache/replacementPolicy.hpp"



LRUReplacementPolicy::LRUReplacementPolicy(const uint32_t& numSets, const uint8_t& associativity)
{
    lruStates.reserve(numSets); // Reserve space for the LRU states based on the number of sets
    for (uint32_t i = 0; i < numSets; ++i)
    {
        initializeSet(associativity); // Initialize the LRU state for each cache set based on the number of sets and associativity
    }
}

void LRUReplacementPolicy::initializeSet(uint8_t associativity)
{
    LRUState state;
    for (uint8_t i = 0; i < associativity; ++i)
    {
        state.queue.push(i); // Initialize the queue with line indices based on the associativity of the cache set
    }
    lruStates.push_back(state); // Add the LRU state for the cache set to the LRU queue
}

uint8_t LRUReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    DEBUG_LOG(debugLog("selezione linea"));
    LRUState& state = lruStates[set.setIndex];

    for(uint8_t i = 0; i < set.lines.size(); ++i)
    {
        if (!set.lines[i].valid) // If there is an invalid line in the set, return its index for replacement
        {
            return i;
        }
    }

    return state.queue.getLastRecent(); // Select the line to replace based on the LRU queue for the cache set

}

void LRUReplacementPolicy::updateOnAccess(CacheSet& set, uint8_t lineIndex)  
{
    lruStates[set.setIndex].queue.touch(lineIndex); // Update the LRU queue on cache access to reflect the most recently used line
}

void LRUReplacementPolicy::onLineLoaded(CacheSet& set, uint8_t lineIndex) 
{
    lruStates[set.setIndex].queue.touch(lineIndex); // Update the LRU queue when a line is loaded into the cache to reflect the most recently used line

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t PLRUTree::selectLineToReplace()
{
    uint8_t node = 0; // Start at the root of the PLRU tree

    while (node < bits.size()) // Traverse the tree until reaching a leaf node
    {
        node = bits[node] ? (2 * node + 2) : (2 * node + 1); // Move left or right based on the bit value at the current node( 1 for right, 0 for left)
    }

    return node - bits.size(); // Return the line index corresponding to the leaf node reached
}

void PLRUTree::updateLine(uint8_t lineIndex)
{
    uint8_t node = lineIndex + bits.size(); // Start at the leaf node corresponding to the accessed line index

    while (node > 0) // Traverse the tree until reaching a leaf node
    {
        uint8_t parent = (node - 1) / 2; // Calculate the parent node index
        bits[parent] = (node == 2 * parent + 1) ? 0 : 1; // Update the bit at the parent node to indicate the direction taken (1 for right, 0 for left) and move up to the parent node
        
        node = parent;
    }
}



PLRUReplacementPolicy::PLRUReplacementPolicy(const uint32_t& numSets, const uint8_t& associativity)
{
    plruTrees.reserve(numSets); // Reserve space for the PLRU trees based on the number of sets
    for (uint32_t i = 0; i < numSets; ++i)
    {
        initializeSet(associativity); // Initialize the PLRU state for each cache set based on the number of sets and associativity
    }
}

void PLRUReplacementPolicy::initializeSet(uint8_t associativity)
{
    PLRUTree tree(associativity); // Create a PLRU tree for the cache set based on the associativity
    for (uint8_t i = 0; i < associativity; ++i)
    {
        tree.updateLine(i); // Initialize the PLRU tree by updating the lines based on the associativity
    }
    plruTrees.push_back(tree); // Add the initialized PLRU tree to the vector
}

uint8_t PLRUReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    auto& tree = plruTrees[set.setIndex]; // Get the PLRU tree for the cache set
    for (uint8_t i = 0; i < set.lines.size(); ++i)
    {
        if (!set.lines[i].valid) // If there is an invalid line in the set, return its index for replacement
        {
            return i;
        }
    }

    return tree.selectLineToReplace(); // Select the line to replace based on the PLRU tree for the cache set
}

void PLRUReplacementPolicy::updateOnAccess(CacheSet& set, uint8_t lineIndex)
{
    auto& tree = plruTrees[set.setIndex]; // Get the PLRU tree for the cache set
    tree.updateLine(lineIndex); // Update the PLRU tree on cache access to reflect the most recently used line
}

void PLRUReplacementPolicy::onLineLoaded(CacheSet& set, uint8_t lineIndex)
{
    auto& tree = plruTrees[set.setIndex]; // Get the PLRU tree for the cache set
    tree.updateLine(lineIndex); // Update the PLRU tree when a line is loaded into the cache to reflect the most recently used line
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t RandomReplacementPolicy::selectLineToReplace(CacheSet& set)
{
    
    
    std::uniform_int_distribution<> dis(0, set.lines.size() - 1); // Uniform distribution to select a random line index from the set

    return dis(gen); // Return a randomly selected line index from the set
}

