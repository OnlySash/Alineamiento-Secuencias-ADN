__kernel void search_pattern(
    __global const char* sequence,  // Argument 0: clSetKernelArg
    __global const char* pattern,   // Argument 1: clSetKernelArg
    __global int* result,           // Argument 2: clSetKernelArg
    __global int* counter,          // Argument 3: clSetKernelArg
    int pattern_len,                // Argument 4: clSetKernelArg
    int sequence_len)                   // Argument 5: clSetKernelArg
{
    int pos = get_global_id(0); 
    if (pos + pattern_len > sequence_len) return;

    int match = 1; 

    // Loop of "sliding window" (char by char comparisson)
    for (int i = 0; i < pattern_len; i++) {
        if (sequence[pos + i] != pattern[i]) {
            match = 0; 
            break;            
        }
    }

    // Atomic Operations to evade Race Conditions
    if (match == 1) {
        int index = atomic_inc(counter);
        result[index] = pos; 
    }
}