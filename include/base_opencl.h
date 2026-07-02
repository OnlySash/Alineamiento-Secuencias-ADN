#ifndef BASE_OPENCL_H
#define BASE_OPENCL_H
#include "base.h"
#include "params.h"

#include <CL/cl.h>

/**
 * \struct opencl_env_t
 * \brief Encapsulates the OpenCL environment state.
 * * Holds all necessary OpenCL objects (device, context, queue, program, and kernel) 
 * to avoid global variables and allow modular, reusable execution of GPU tasks.
 */
typedef struct {
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
} opencl_env_t;

/**
 * \brief Reads the OpenCL kernel source code from a file.
 * * Opens the specified file, determines its size, allocates memory, and loads the 
 * entire content into a null-terminated string for Just-In-Time (JIT) compilation.
 * \param filename Path to the .cl file containing the kernel source.
 * \return Pointer to the dynamically allocated string containing the source code.
 */
char* read_kernel_source(const char* filename);

/**
 * \brief Initializes the OpenCL environment and compiles the kernel.
 * * Discovers an available GPU platform, creates a context and a command queue, 
 * loads the kernel source using JIT compilation, and extracts the target kernel function.
 * \param kernel_path Path to the OpenCL kernel source file (e.g., "src/kernel.cl").
 * \return An initialized opencl_env_t structure ready for execution.
 */
opencl_env_t init_opencl_env(const char* kernel_path);

/**
 * \brief Executes the DNA pattern search on the GPU using OpenCL.
 * * Allocates VRAM buffers, transfers the DNA string and patterns, configures 
 * kernel arguments, and dispatches the execution to the GPU. Retrieves and 
 * processes the matching results using atomic counters to prevent race conditions.
 * \param env Pointer to the initialized OpenCL environment structure.
 * \param dna_string Pointer to the main DNA string.
 * \param dna_string_length Length of the main DNA string.
 * \param patterns Array of pattern_t structures to search.
 * \param k_patterns Number of patterns in the array.
 */
void search_patterns_opencl(opencl_env_t* env, const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns);

/**
 * \brief Releases all OpenCL resources allocated in the environment.
 * * Safely destroys the kernel, program, command queue, and context to prevent 
 * memory leaks in the GPU driver.
 * \param env Pointer to the OpenCL environment structure to be cleaned up.
 * IMPORTANT: clRelease* functions must be called in the reverse order of creation to ensure proper cleanup.
 */
void finalize_opencl(opencl_env_t* env);

/**
 * \brief Main orchestrator for the OpenCL execution mode.
 * * Generates the DNA and pattern data based on user parameters, initializes the OpenCL 
 * environment, executes the search on the GPU, prints the results, and performs cleanup.
 * \param params Structure containing the execution parameters (DNA length, patterns, etc.).
 */
void run_opencl(params_t params);

#endif // BASE_OPENCL_H
