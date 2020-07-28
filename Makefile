LDFLAGS = `pkg-config --static --libs glfw3` -lvulkan

SPVS = vertex.spv fragment.spv

vulkanExamples: $(SPVS) obj/main.o
	g++ obj/main.o -o vulkanExamples $(LDFLAGS)

clean:
	rm *.spv
	rm *.o
	rm -f vulkanExamples

obj/main.o: examples/main.cpp
	g++ -c $^ -o $@

fragment.spv: fragment.glsl
	glslc -fshader-stage=fragment $^ -o $@

vertex.spv: vertex.glsl
	glslc -fshader-stage=vertex $^ -o $@
