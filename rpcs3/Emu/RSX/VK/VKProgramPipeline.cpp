#include "stdafx.h"
#include "VKHelpers.h"

namespace vk
{
	namespace glsl
	{
		program::program(VkDevice dev, VkPipeline p, const std::vector<program_input> &vertex_input, const std::vector<program_input>& fragment_inputs)
			: m_device(dev), pipeline(p)
		{
			load_uniforms(glsl::program_domain::glsl_vertex_program, vertex_input);
			load_uniforms(glsl::program_domain::glsl_vertex_program, fragment_inputs);
		}

		program::~program()
		{
			LOG_ERROR(RSX, "Program destructor invoked!");
			vkDestroyPipeline(m_device, pipeline, nullptr);
		}

		program& program::load_uniforms(program_domain domain, const std::vector<program_input>& inputs)
		{
			std::vector<program_input> store = uniforms;
			uniforms.resize(0);

			for (auto &item : store)
			{
				uniforms.push_back(item);
			}

			for (auto &item : inputs)
				uniforms.push_back(item);

			return *this;
		}

		bool program::has_uniform(std::string uniform_name)
		{
			for (auto &uniform : uniforms)
			{
				if (uniform.name == uniform_name)
					return true;
			}

			return false;
		}

		void program::bind_uniform(VkDescriptorImageInfo image_descriptor, std::string uniform_name, VkDescriptorSet &descriptor_set)
		{
			for (auto &uniform : uniforms)
			{
				if (uniform.name == uniform_name)
				{
					VkWriteDescriptorSet descriptor_writer = {};
					descriptor_writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_writer.dstSet = descriptor_set;
					descriptor_writer.descriptorCount = 1;
					descriptor_writer.pImageInfo = &image_descriptor;
					descriptor_writer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptor_writer.dstArrayElement = 0;
					descriptor_writer.dstBinding = uniform.location + TEXTURES_FIRST_BIND_SLOT;

					vkUpdateDescriptorSets(m_device, 1, &descriptor_writer, 0, nullptr);
					return;
				}
			}

			throw EXCEPTION("texture not found");
		}

		void program::bind_uniform(VkDescriptorBufferInfo buffer_descriptor, uint32_t binding_point, VkDescriptorSet &descriptor_set)
		{
			VkWriteDescriptorSet descriptor_writer = {};
			descriptor_writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writer.dstSet = descriptor_set;
			descriptor_writer.descriptorCount = 1;
			descriptor_writer.pBufferInfo = &buffer_descriptor;
			descriptor_writer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_writer.dstArrayElement = 0;
			descriptor_writer.dstBinding = binding_point;

			vkUpdateDescriptorSets(m_device, 1, &descriptor_writer, 0, nullptr);
		}

		void program::bind_uniform(const VkBufferView &buffer_view, const std::string &binding_name, VkDescriptorSet &descriptor_set)
		{
			for (auto &uniform : uniforms)
			{
				if (uniform.name == binding_name)
				{
					VkWriteDescriptorSet descriptor_writer = {};
					descriptor_writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_writer.dstSet = descriptor_set;
					descriptor_writer.descriptorCount = 1;
					descriptor_writer.pTexelBufferView = &buffer_view;
					descriptor_writer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
					descriptor_writer.dstArrayElement = 0;
					descriptor_writer.dstBinding = uniform.location + VERTEX_BUFFERS_FIRST_BIND_SLOT;

					vkUpdateDescriptorSets(m_device, 1, &descriptor_writer, 0, nullptr);
					return;
				}
			}
			throw EXCEPTION("vertex buffer not found");
		}
	}
}
