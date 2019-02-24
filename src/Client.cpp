#include "Client.h"

namespace v3d{
	void Client::RenderScene(std::shared_ptr<FramebufferGL> fbo)
	{
		v3d::dx::SceneLoader loader(filename, std::move(fbo), dx::winW, dx::winH);
	    loader.initData();
	    loader.initScene();
	    loader.updateView();
	    loader.updateRenderer();

	    // create renderer

	//    auto renderer = std::make_shared<RegularGridPipelineGL>();
	//    renderer->setScene(loader.getSceneGrid());

	//    auto renderer = std::make_shared<TetraGridPipelineGL>();
	//    renderer->setScene(loader.getSceneTets());

//	    auto renderer = loader.getRendererGrid();
//	//    auto renderer = loader.getRendererTets();
//	    renderer->setFramebufferObject(fbo->sharedFramebufferObject());
//	    renderer->resize(dx::winW, dx::winH);

	    // render
	    loader.render();

	    // get framebuffer image
	    std::vector<unsigned char> buffer(dx::winW * dx::winH * 4);
	    GLuint currFbo = GLFramebufferObject::currentDrawBinding();
	    fbo->bind();
	    V3D_GL_PRINT_ERRORS();
	    GLint readBuffer;
	    glGetIntegerv(GL_READ_BUFFER, &readBuffer);
	    glReadBuffer(GL_COLOR_ATTACHMENT0);
	    glReadPixels(0, 0, dx::winW, dx::winH, GL_BGRA, GL_UNSIGNED_BYTE, &buffer[0]);
	    glReadBuffer(readBuffer);
	    V3D_GL_PRINT_ERRORS();
	    GLFramebufferObject::bind(currFbo);
	    V3D_GL_PRINT_ERRORS();
	    for (int i = 0; i < dx::winW * dx::winH; i++) buffer[i * 4 + 3] = 255;

	    // save
	    QImage img = QImage(&buffer[0], dx::winW, dx::winH, QImage::Format_RGB32).mirrored(false, true);
	    std::string filename = "image" + std::to_string(id) + ".PNG";
	    img.save(filename.c_str(), 0, -1);
	    std::cout << "save file as " << filename << std::endl;
	}
}	

