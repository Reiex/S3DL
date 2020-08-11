CFLAGS = -Iinclude -g
LDFLAGS = `pkg-config --static --libs glfw3` -lvulkan

SPVS = vertex.spv fragment.spv
OBJS = obj/main.o \
	   obj/Instance.o

vulkanExamples: $(SPVS) $(OBJS)
	g++ $(CFLAGS) $(OBJS) -o vulkanExamples $(LDFLAGS)

clean:
	-rm *.spv
	-rm obj/*.o
	-rm -f vulkanExamples

obj/main.o: examples/main.cpp
	g++ $(CFLAGS) -c $^ -o $@

obj/%.o: src/S3DL/%.cpp
	g++ $(CFLAGS) -c $^ -o $@

fragment.spv: fragment.glsl
	glslc -fshader-stage=fragment $^ -o $@

vertex.spv: vertex.glsl
	glslc -fshader-stage=vertex $^ -o $@
