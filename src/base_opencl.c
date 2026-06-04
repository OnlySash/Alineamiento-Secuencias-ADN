#include "base_opencl.h"


// Este es el "Kernel", el código que se ejecutará en paralelo dentro de la GPU
const char *kernel_source = 
"__kernel void hola_kernel(__global char *datos) {  \n"
"    int id = get_global_id(0);                     \n"
"    datos[id] = datos[id] + 1;                     \n" // Modifica el carácter en paralelo
"}                                                  \n";

int main() {
    // 1. Datos de entrada (Hola Mundo en código ASCII)
    char mensaje[] = "Hola Mundo de OpenCL!";
    int longitud = sizeof(mensaje);
    printf("Texto original en CPU: %s\n", mensaje);

    // 2. Variables de entorno de OpenCL
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    cl_mem mem_obj = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;

    // 3. Obtener plataformas y dispositivos disponibles (GPU o CPU)
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    // 4. Crear el contexto de OpenCL y la cola de comandos
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);

    // 5. Crear el buffer de memoria en la GPU y copiar los datos del CPU
    mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, longitud * sizeof(char), mensaje, &ret);

    // 6. Crear y compilar el Kernel en tiempo de ejecución
    program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, NULL, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // 7. Crear el objeto del kernel
    kernel = clCreateKernel(program, "hola_kernel", &ret);

    // 8. Pasar el buffer como argumento al kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&mem_obj);

    // 9. Ejecutar el kernel en la GPU (un hilo por cada carácter)
    size_t global_item_size = longitud; 
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, NULL, 0, NULL, NULL);

    // 10. Leer el buffer modificado de vuelta al CPU
    ret = clEnqueueReadBuffer(command_queue, mem_obj, CL_TRUE, 0, longitud * sizeof(char), mensaje, 0, NULL, NULL);

    // 11. El CPU procesa el resultado de la GPU
    printf("Texto modificado por GPU: %s\n", mensaje);
    
    for(int i = 0; i < longitud - 1; i++) {
        mensaje[i] = mensaje[i] - 1; // Revertimos el +1 para verificar
    }
    printf("Texto restaurado por CPU: %s\n", mensaje);

    // 12. Limpieza de recursos
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    return 0;
}
