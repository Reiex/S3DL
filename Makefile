###############################################################################
# ENVIRONMENT VARIABLES                                                       #
###############################################################################


# Source directories
SRC_LIBRARY_DIR = src/S3DL
SRC_TESTS_DIR = tests
SRC_EXAMPLES_DIR = examples

# Include directory
INCLUDE_DIR = include

# .obj directories
OBJ_DIR = obj
OBJ_LIBRARY_DIR = $(OBJ_DIR)/S3DL
OBJ_TESTS_DIR = $(OBJ_DIR)/tests
OBJ_EXAMPLES_DIR = $(OBJ_DIR)/examples

# Compiled library directory
LIB_DIR = lib

# .obj lists
LIBRARY_OBJS = $(OBJ_LIBRARY_DIR)/Instance.o \
			   $(OBJ_LIBRARY_DIR)/Window.o \
			   $(OBJ_LIBRARY_DIR)/RenderTarget.o \
			   $(OBJ_LIBRARY_DIR)/RenderWindow.o \
			   $(OBJ_LIBRARY_DIR)/Device.o \
			   $(OBJ_LIBRARY_DIR)/Swapchain.o \
			   $(OBJ_LIBRARY_DIR)/Attachment.o \
			   $(OBJ_LIBRARY_DIR)/Subpass.o \
			   $(OBJ_LIBRARY_DIR)/Dependency.o \
			   $(OBJ_LIBRARY_DIR)/RenderPass.o \
			   $(OBJ_LIBRARY_DIR)/Shader.o \
			   $(OBJ_LIBRARY_DIR)/PipelineLayout.o \
			   $(OBJ_LIBRARY_DIR)/Pipeline.o \
			   $(OBJ_LIBRARY_DIR)/Vertex.o \
			   $(OBJ_LIBRARY_DIR)/Buffer.o \
			   $(OBJ_LIBRARY_DIR)/stb/stb_image.o \
			   $(OBJ_LIBRARY_DIR)/stb/stb_image_write.o \
			   $(OBJ_LIBRARY_DIR)/TextureData.o \
			   $(OBJ_LIBRARY_DIR)/TextureSampler.o \
			   $(OBJ_LIBRARY_DIR)/Texture.o \
			   $(OBJ_LIBRARY_DIR)/Framebuffer.o
TESTS_OBJS = $(OBJ_TESTS_DIR)/main.o
EXAMPLES_OBJS = $(OBJ_EXAMPLES_DIR)/main.o \
				$(OBJ_EXAMPLES_DIR)/viking_room.o \
				$(OBJ_EXAMPLES_DIR)/master.o \
			    $(OBJ_EXAMPLES_DIR)/tiny_obj_loader/tiny_obj_loader.o

# Compiler
CC = g++
# Compiler options
CFLAGS = -I$(INCLUDE_DIR) -g -I/home/reiex/4DViews/Dev/mvworkflow/PiEZo/Module
# Linker options
LDFLAGS = -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR)
# Libraries linked
LDLIBS = `pkg-config --static --libs glfw3` -lvulkan -lS3DL -lagafar -l4dvcurt -lpomo -lmodule-bagel


# Examples shaders sources directory
S_SRC_EXAMPLES_DIR = $(SRC_EXAMPLES_DIR)/shaders
# Examples SPIR-V directory
S_SPV_EXAMPLES_DIR = $(SRC_EXAMPLES_DIR)/spvs

# Examples SPIR-V list
S_EXAMPLES_SPVS = $(S_SPV_EXAMPLES_DIR)/main.vert.spv \
				  $(S_SPV_EXAMPLES_DIR)/main.frag.spv \
				  $(S_SPV_EXAMPLES_DIR)/subpass.vert.spv \
				  $(S_SPV_EXAMPLES_DIR)/subpass.frag.spv

# Shader compiler
S_CC = glslc
# Shader compiler options
S_CFLAGS = --target-env=vulkan1.1


###############################################################################
# GENERAL TARGETS                                                             #
###############################################################################


all: clean folders S3DL tests examples

check: tests
	valgrind ./S3DLTests

# docs:
# 	doxygen doc/doxyfilerc

clean:
	-find $(OBJ_DIR) -type f -exec rm -rf \{\} \;
	-find $(LIB_DIR) -type f -exec rm -rf \{\} \;
	-rm -rf $(S_SPV_EXAMPLES_DIR)/*
	-rm -rf S3DLExamples S3DLTests


###############################################################################
# GROUP TARGETS                                                               #
###############################################################################


S3DL: $(LIB_DIR)

examples: $(S_EXAMPLES_SPVS) $(LIB_DIR) $(EXAMPLES_OBJS)
	$(CC) $(EXAMPLES_OBJS) -o S3DLExamples $(LDFLAGS) $(LDLIBS)

tests: $(LIB_DIR) $(TESTS_OBJS)
	$(CC) $(TESTS_OBJS) -o S3DLTests $(LDFLAGS) $(LDLIBS)

folders:
	-rm -rf $(LIB_DIR) $(OBJ_DIR)
	mkdir $(OBJ_DIR) $(OBJ_LIBRARY_DIR) $(OBJ_LIBRARY_DIR)/stb $(OBJ_TESTS_DIR) $(OBJ_EXAMPLES_DIR) $(OBJ_EXAMPLES_DIR)/tiny_obj_loader
	mkdir $(LIB_DIR)


###############################################################################
# LIBRARY BUILDING TARGETS                                                    #
###############################################################################


$(LIB_DIR): $(LIBRARY_OBJS)
	-rm -rf $(LIB_DIR)/libS3DL.so
	$(CC) -shared -o $(LIB_DIR)/libS3DL.so $(LIBRARY_OBJS)

$(OBJ_LIBRARY_DIR)/%.o: $(SRC_LIBRARY_DIR)/%.cpp
	$(CC) $(CFLAGS) -fpic -c $< -o $@


###############################################################################
# UNIT COMPILATION TARGETS                                                    #
###############################################################################


$(OBJ_TESTS_DIR)/%.o: $(SRC_TESTS_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_EXAMPLES_DIR)/%.o: $(SRC_EXAMPLES_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(S_SPV_EXAMPLES_DIR)/%.vert.spv: $(S_SRC_EXAMPLES_DIR)/%.vert.glsl
	$(S_CC) $(S_CFLAGS) -fshader-stage=vertex $< -o $@

$(S_SPV_EXAMPLES_DIR)/%.tesc.spv: $(S_SRC_EXAMPLES_DIR)/%.tesc.glsl
	$(S_CC) $(S_CFLAGS) -fshader-stage=tesscontrol $< -o $@

$(S_SPV_EXAMPLES_DIR)/%.tese.spv: $(S_SRC_EXAMPLES_DIR)/%.tese.glsl
	$(S_CC) $(S_CFLAGS) -fshader-stage=tesseval $< -o $@

$(S_SPV_EXAMPLES_DIR)/%.geom.spv: $(S_SRC_EXAMPLES_DIR)/%.geom.glsl
	$(S_CC) $(S_CFLAGS) -fshader-stage=geometry $< -o $@

$(S_SPV_EXAMPLES_DIR)/%.frag.spv: $(S_SRC_EXAMPLES_DIR)/%.frag.glsl
	$(S_CC) $(S_CFLAGS) -fshader-stage=fragment $< -o $@
