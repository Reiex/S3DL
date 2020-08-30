CFLAGS = -Iinclude -g
LDFLAGS = `pkg-config --static --libs glfw3` -lvulkan

SPVS = vertex.spv fragment.spv subpassVertex.spv subpassFragment.spv
OBJS = obj/main.o \
	   obj/Instance.o \
	   obj/Window.o \
	   obj/RenderTarget.o \
	   obj/RenderWindow.o \
	   obj/Device.o \
	   obj/Swapchain.o \
	   obj/Attachment.o \
	   obj/Subpass.o \
	   obj/Dependency.o \
	   obj/RenderPass.o \
	   obj/Shader.o \
	   obj/Pipeline.o \
	   obj/Vertex.o \
	   obj/Buffer.o \
	   obj/stb/stb_image.o \
	   obj/stb/stb_image_write.o \
	   obj/TextureData.o \
	   obj/TextureSampler.o \
	   obj/Texture.o \
	   obj/Framebuffer.o

vulkanExamples: $(SPVS) $(OBJS)
	g++ $(CFLAGS) $(OBJS) -o vulkanExamples $(LDFLAGS)

shaders: $(SPVS)

clean:
	-rm *.spv
	-rm obj/*.o
	-rm -f vulkanExamples

obj/main.o: examples/main.cpp
	g++ $(CFLAGS) -c $^ -o $@

obj/%.o: src/S3DL/%.cpp
	g++ $(CFLAGS) -c $^ -o $@

vertex.spv: vertex.glsl
	glslc -fshader-stage=vertex $^ -o $@

subpassVertex.spv: subpassVertex.glsl
	glslc -fshader-stage=vertex $^ -o $@

fragment.spv: fragment.glsl
	glslc -fshader-stage=fragment $^ -o $@

subpassFragment.spv: subpassFragment.glsl
	glslc -fshader-stage=fragment $^ -o $@
