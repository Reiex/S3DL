CFLAGS = -Iinclude
LDFLAGS = `pkg-config --static --libs glfw3` -lvulkan

SPVS = vertex.spv fragment.spv

vulkanExamples: $(SPVS) obj/main.o obj/Device.o obj/RenderWindow.o obj/Window.o
	g++ $(CFLAGS) obj/main.o obj/Device.o obj/RenderWindow.o obj/Window.o -o vulkanExamples $(LDFLAGS)

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
