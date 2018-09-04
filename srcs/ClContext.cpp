
#include "ParticleGenerator.hpp"
#include "ClContext.hpp"

// struct ClParticle {
//     cl_float3 Position;
// 	cl_float3 Velocity;
//     cl_float4 Color;
// };

ClContext *ClContext::getInstance()
{
	if (_singleton == NULL)
	{
		std::cout << "creating CL singleton" << '\n';
		_singleton = new ClContext();
	}
	else
	{
		std::cout << "CL singleton already created" << '\n';
	}
	return _singleton;
}

cl_float ClContext::length_f3(cl_float3 vec)
{
	return (sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}

void ClContext::kill()
{
	if (_singleton != NULL)
	{
		delete _singleton;
		_singleton = NULL;
	}
}

ClContext::~ClContext()
{
	std::cout << "ClContext destructor" << '\n';
}

ClContext::ClContext() 
{
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.size() == 0)
	{
		std::cout << " No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	this->default_platform = all_platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	//get default device of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if (all_devices.size() == 0)
	{
		std::cout << " No devices found. Check OpenCL installation!\n";
		exit(1);
	}
	this->default_device = all_devices[0];
	std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

	this->context = cl::Context({default_device});

	// kernel calculates for each element C=A+B
	this->kernel_code =
	"typedef struct __attribute__ ((packed)) _Cl_Particle"
	"{"
    "	float3 Position;"
	"	float3 Velocity;"
    "	float4 Color;"
	"} ClParticle;"
	"   void kernel simple_add(global const ClParticle *A, global const ClParticle *B, global ClParticle *C){       "
	"	int i = get_global_id(0);"
	"       C[i].Position = A[i].Position + B[i].Position;"
	"       C[i].Velocity = A[i].Velocity + B[i].Velocity;"
	"       C[i].Color = A[i].Color + B[i].Color;"
	"   }      ";
	this->sources.push_back({this->kernel_code.c_str(), this->kernel_code.length()});

	cl::Program program(context, this->sources);
	if (program.build({default_device}) != CL_SUCCESS)
	{
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		exit(1);
	}

	ClParticle arrParticlesA[10];
	for (int i = 0; i < 10; i++)
	{
		arrParticlesA[i].Position.x = 50 * i + 0;
		arrParticlesA[i].Position.y = 50 * i + 1;
		arrParticlesA[i].Position.z = 50 * i + 2;
		arrParticlesA[i].Velocity.x = 50 * i + 3;
		arrParticlesA[i].Velocity.y = 50 * i + 4;
		arrParticlesA[i].Velocity.z = 50 * i + 5;
		arrParticlesA[i].Color.x = 50 * i + 6;
		arrParticlesA[i].Color.y = 50 * i + 7;
		arrParticlesA[i].Color.z = 50 * i + 8;
		arrParticlesA[i].Color.w = 50 * i + 9;
	}

	ClParticle arrParticlesB[10];
	for (int i = 0; i < 10; i++)
	{
		arrParticlesB[i].Position.x = 50 * i + 10;
		arrParticlesB[i].Position.y = 50 * i + 11;
		arrParticlesB[i].Position.z = 50 * i + 12;
		arrParticlesB[i].Velocity.x = 50 * i + 13;
		arrParticlesB[i].Velocity.y = 50 * i + 14;
		arrParticlesB[i].Velocity.z = 50 * i + 15;
		arrParticlesB[i].Color.x = 50 * i + 16;
		arrParticlesB[i].Color.y = 50 * i + 17;
		arrParticlesB[i].Color.z = 50 * i + 18;
		arrParticlesB[i].Color.w = 50 * i + 19;
	}

	// create buffers on the device
	cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(ClParticle) * 10);
	cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(ClParticle) * 10);
	cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(ClParticle) * 10);



	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context, default_device);

	//write arrays A and B to the device
	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(ClParticle) * 10, arrParticlesA);
	queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(ClParticle) * 10, arrParticlesB);


	//alternative way to run the kernel
	cl::Kernel kernel_add = cl::Kernel(program, "simple_add");
	kernel_add.setArg(0, buffer_A);
	kernel_add.setArg(1, buffer_B);
	kernel_add.setArg(2, buffer_C);
	queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(10), cl::NullRange);
	queue.finish();

	ClParticle C[10];
	//read result C from the device to array C
	queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(ClParticle) * 10, C);

	C[1].Position.v4 = C[2].Position.v4 + C[1].Position.v4;

	std::cout << " result: \n";
	// for (int i = 0; i < 10; i++)
	// {
	// 	std::cout << "Position de C = {" << C[i].Position.x << ", " << C[i].Position.y << ", " << C[i].Position.z << "} \n";
	// 	std::cout << "Velocity de C = {" << C[i].Velocity.x << ", " << C[i].Velocity.y << ", " << C[i].Velocity.z << "} \n";
	// 	std::cout << "Color de C = {" << C[i].Color.x << ", " << C[i].Color.y << ", " << C[i].Color.z << "} \n";
	// 	std::cout << "--------------------------------------------------------\n\n";
	// }
}

ClContext *ClContext::_singleton = NULL;