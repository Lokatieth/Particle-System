#ifndef CL_CONTEXT_HPP
#define CL_CONTEXT_HPP
// #include "ParticleSystem.hpp"

#include <vector>
#include <iostream>
#include "cl.hpp"

#define __CL_ENABLE_EXCEPTIONS

typedef struct __attribute__ ((packed)) _Cl_Particle
{
    cl_float3 Position;
	cl_float3 Velocity;
    cl_float4 Color;

} ClParticle;


class ClContext
{
public: 
	ClContext();
	~ClContext();

	static ClContext *getInstance();
	static void kill();
	static cl_float length_f3(cl_float3 vec);

	void calculateAttraction(ClParticle *array, ClParticle *result, const float delta, const cl_float3 attractor, const cl_float3 retractor);
	cl::Platform default_platform;
	cl::Device default_device;
	cl::Context context;
	cl::Program::Sources sources;
	std::string kernel_code;
	cl::Program program;

private:
	static ClContext *_singleton;
};

#endif