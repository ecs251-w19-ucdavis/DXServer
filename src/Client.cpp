#include "Client.h"

namespace v3d{
	void Client::RenderScene(std::shared_ptr<FramebufferGL> fbo)
	{
		v3d::dx::SceneHandler loader(filename, std::move(fbo), dx::winW, dx::winH);
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
		auto buffer = loader.copyRenderedImage();

		// save
	    QImage img = QImage(&(*buffer)[0], dx::winW, dx::winH, QImage::Format_RGB32).mirrored(false, true);
	    std::string filename = "image" + std::to_string(id) + ".PNG";
	    img.save(filename.c_str(), 0, -1);
	    std::cout << "save file as " << filename << std::endl;
	}
}	

