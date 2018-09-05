
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


void ClContext::calculateAttraction(
	ClParticle *array,
	ClParticle *result,
	const float delta,
	const cl_float3 attractor, 
	const cl_float3 retractor)
{


	
	

	cl::Buffer buffer_part(context, CL_MEM_READ_WRITE, sizeof(ClParticle) * 15000);
	cl::Buffer buffer_result(context, CL_MEM_READ_WRITE, sizeof(ClParticle) * 15000);
	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context, default_device);
	//write arrays A and B to the device
	queue.enqueueWriteBuffer(buffer_part, CL_TRUE, 0, sizeof(ClParticle) * 15000, array);
	queue.enqueueWriteBuffer(buffer_result, CL_TRUE, 0, sizeof(ClParticle) * 15000, result);
	// queue.enqueueWriteBuffer(buffer_dt, CL_TRUE, 0, sizeof(float), delta);
	// queue.enqueueWriteBuffer(buffer_att, CL_TRUE, 0, sizeof(cl_float3), att);
	// queue.enqueueWriteBuffer(buffer_ret, CL_TRUE, 0, sizeof(cl_float3), ret);


	//alternative way to run the kernel
	cl::Kernel kernel_add = cl::Kernel(program, "particle_update");
	kernel_add.setArg(0, *array);
	kernel_add.setArg(1, *result);
	kernel_add.setArg(2, delta);
	kernel_add.setArg(3, attractor);
	kernel_add.setArg(4, retractor);
	queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(15000), cl::NullRange);
	queue.finish();

	//read result C from the device to array C
	queue.enqueueReadBuffer(buffer_result, CL_TRUE, 0, sizeof(ClParticle) * 15000, result);


	std::cout << " result: \n";
	for (int i = 0; i < 10; i++)
	{
		std::cout << "Position de array = {" << result[i].Position.x << ", " << result[i].Position.y << ", " << result[i].Position.z << "} \n";
		std::cout << "Velocity de result = {" << result[i].Velocity.x << ", " << result[i].Velocity.y << ", " << result[i].Velocity.z << "} \n";
		std::cout << "Color de result = {" << result[i].Color.x << ", " << result[i].Color.y << ", " << result[i].Color.z << "} \n";
		std::cout << "--------------------------------------------------------\n\n";
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

	this->kernel_code =
		"typedef struct __attribute__ ((packed)) Cl_Particle"
		"{"
	    "	float3 Position;"
		"	float3 Velocity;"
	    "	float4 Color;"
		"} ClParticle;"
		

		"static float f3_length(float3 vec)"
		"{"
		"	return (sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));"
		"}"

		"void kernel particle_update("
		"	global ClParticle *P,"
		"	global ClParticle *R,"
		"	 float const dt,"
		"	 const float3 attractor,"
		"	 const float3 retractor ){       "


		"		int i = get_global_id(0);"
		"		float acc;"
		"		float distToAttr;"
		"		float velocityLength;"
		"		float speedRatio;"
		"		float3 distAttractor;"

		"		distAttractor = P[i].Position - attractor; "
		"		distToAttr = f3_length(distAttractor);"
		"		acc = 2 * (distToAttr / 10);"

		// "		R[i].Velocity.x = (P[i].Velocity.x + (distAttractor.x > 0 ? acc : -acc)) * 0.98;"
		// "		R[i].Velocity.y = (P[i].Velocity.y + (distAttractor.y > 0 ? acc : -acc)) * 0.98;"
		// "		R[i].Velocity.z = (P[i].Velocity.z + (distAttractor.z > 0 ? acc : -acc)) * 0.98;"


		"  		R[i].Velocity.x = 10;"
		"		R[i].Velocity.y = 10;"
		"		R[i].Velocity.z = 10;"

		"		velocityLength = f3_length(P[i].Velocity);"

		"		R[i].Position = P[i].Position - (P[i].Velocity * dt / 2.0f);"

		"		speedRatio = velocityLength / 3;"

		"		R[i].Color.x = (1 - speedRatio) * (1 - speedRatio);"
		"		R[i].Color.y = 0.1 + 0.9 * (speedRatio / 2);"
		"		R[i].Color.z = 0.1 + 0.9 * (speedRatio);"
		"   }      ";
		this->sources.push_back({this->kernel_code.c_str(), this->kernel_code.length()});

	cl::Program program(context, this->sources);
	if (program.build({default_device}) != CL_SUCCESS)
	{
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		exit(1);
	}

	// kernel calculates for each element C=A+B
	
}

ClContext *ClContext::_singleton = NULL;