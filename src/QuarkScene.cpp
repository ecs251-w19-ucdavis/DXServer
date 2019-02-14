//
// Created by qadwu on 1/10/19.
//

#ifdef _WIN32
#define _DATA_ "C:\\Users\\wilson\\Documents\\Work\\ViDi\\data\\vorts1.data"
#else
//#define _DATA_ "/home/qadwu/Work/data/vidi3d/vorts1.data"
#define _DATA_ "/Users/qwu/Work/projects/vidi/data/vorts1.data"
#endif

#include <QImage>
#include <QFile>

#include "Util/Library.h"
#include "Util/JsonParser.h"
#include "Renderer/IRenderer.h"

#include "Data/RegularGridLoader.h"
#include "Scene/Medium/RegularGridDataGL.h"
#include "Scene/Geometry/RegularGridVolumeGL.h"
#include "Scene/TransferFunction/TransferFunction.h"
#include "Scene/TransferFunction/OcclusionTransferFunction.h"
#include "Renderer/RegularGridSceneGL.h"
#include "Renderer/RegularGridPipelineGL.h"
#include "Renderer/FramebufferGL.h"
#include "Util/Camera.h"
#include "Util/SourceCodeManager.h"

#include "QuarkGLHeader.h"

namespace v3d {

void loadModule()
{
    auto &repo = *v3d::LibraryRepository::GetInstance();
    repo.addDefaultLibrary();
    if (!repo.libraryExists("vidi3d_core")) {
        repo.add("vidi3d_core");
    }
    if (!repo.libraryExists("vidi3d_module_opengl")) {
        repo.add("vidi3d_module_opengl");
    }
}

QString loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        throw v3d::Error("[error] Cannot open the file \"" + std::string(fileName.toLatin1().constData()) + "\"");
    }
    qint64 size = file.size();
    std::unique_ptr<char[]> buffer(new char[size + 1]);
    QDataStream ds(&file);
    ds.readRawData(buffer.get(), int(size));
    buffer[size] = '\0';
    return QString(buffer.get());
}

void loadAllShaders()
{
    QStringList fileNames;
    fileNames
        << "Shader/AdvancedRayCast.frag"
        << "Shader/ModularRayCast.frag"
        << "Shader/Occlusion.comp"
        << "Shader/occlusionMake.comp"
        << "Shader/PolyLighting.frag"
        << "Shader/PolyLighting.vert"
        << "Shader/PolySingleColor.frag"
        << "Shader/PolySingleColor.vert"
        << "Shader/Position.frag"
        << "Shader/RayIntersect.frag"
        << "Shader/RayIntersect.vert"
        << "Shader/RegularGridRayCast.frag"
        << "Shader/RegularGridRayCast.vert"
        << "Shader/RegularGridSlice.frag"
        << "Shader/RegularGridSlice.vert"
        << "Shader/SimpleRegularGridRayCast.frag"
        << "Shader/SimpleRegularGridRayCast.vert"
        << "Shader/skipMake.comp"
        << "Shader/TetraCellWalk.frag"
        << "Shader/TetraCellWalk.vert"
        << "Shader/TetraEntryExit.frag"
        << "Shader/TetraEntryExit.vert"
        << "Shader/TetraExit.frag"
        << "Shader/TetraExit.vert"
        << "Shader/TetraFar.frag"
        << "Shader/TetraFar.vert"
        << "Shader/TetraFirstEntry.frag"
        << "Shader/TetraFirstEntry.vert"
        << "Shader/TetraGridRayCast.frag"
        << "Shader/TetraGridRayCast.vert"
        << "Shader/parts/castRay.frag"
        << "Shader/parts/castRay_emptySpaceSkipping.frag"
        << "Shader/parts/exAmbOcclusion.frag"
        << "Shader/parts/exAOasShadow.frag"
        << "Shader/parts/exBPhong.frag"
        << "Shader/parts/exLight.frag"
        << "Shader/parts/exLightless.frag"
        << "Shader/parts/exNoAO.frag"
        << "Shader/parts/exNoShadow.frag"
        << "Shader/parts/exPhong.frag"
        << "Shader/parts/exSampling.frag"
        << "Shader/parts/exShading.frag"
        << "Shader/parts/exShadow.frag"
        << "Shader/parts/Shadow_LightVolume.glsl"
        << "Shader/parts/skip_homogeneous.comp"
        << "Shader/parts/skip_transparent.comp"
        << "Shader/parts/testShader.frag"
        << "Shader/parts/TF_preint.frag"
        << "Shader/parts/TF_regular.frag"
        << "Shader/parts/TF_TF2D.glsl"
        << "Shader/parts/TFelem_none.frag"
        << "Shader/parts/TFelem_Phong.frag"
        << "Shader/Module/CastRay.glsl"
        << "Shader/Module/CastRay_EmptySpaceSkip.glsl"
        << "Shader/Module/Classify_Preint.glsl"
        << "Shader/Module/Classify_PreintShade.glsl"
        << "Shader/Module/Classify_Shade.glsl"
        << "Shader/Module/Classify_TF2D.glsl"
        << "Shader/Module/Classify_TF2DShade.glsl"
        << "Shader/Module/Data.glsl"
        << "Shader/Module/Data_2D.glsl"
        << "Shader/Module/Light.glsl"
        << "Shader/Module/LightVolume.glsl"
        << "Shader/Module/OcclusionVolume.glsl"
        << "Shader/Module/RayStartDist_Preint.glsl"
        << "Shader/Module/Shade.glsl"
        << "Shader/Module/Shade_Preint.glsl"
        << "Shader/Module/TF.glsl"
        << "Shader/Module/TF_2D.glsl"
        << "Shader/Module/TF_Preint.glsl"
        << "Shader/Module/TricubicInterp.glsl";
    for (auto &fn : fileNames)
    {
        QString srcCode = loadFile(QString(":/%1").arg(fn));
        SourceCodeManager::get().setSourceCode(fn.toStdString(), srcCode.toStdString());
    }
}

void createScene(int *argc, const char **argv, std::shared_ptr<FramebufferGL> &&fbo)
{
    loadModule();
    loadAllShaders();

    // load data
    auto dataSrc = std::make_shared<RegularGridLoader>();
    auto dataOut = std::make_shared<RegularGridDataGL>();
    dataSrc->setFileName(_DATA_);
    dataSrc->setOffset(0);
    dataSrc->setDimensions({128, 128, 128});
    dataSrc->setType(V3D_FLOAT);
    dataSrc->setEndian(V3D_LITTLE_ENDIAN);
    dataSrc->setFileUpperLeft(false);
    dataSrc->setOutputData(dataOut.get());
    dataSrc->update();
    dataOut->loadGL();

    // setup volume
    const ivec3 dim = dataOut->dimensions();
    const vec3 gridOrigin = dataOut->origin();
    const vec3 gridSpacing = dataOut->spacing();
    const vec3 gridExtents = vec3(dim - ivec3(1)) * gridSpacing * 0.5f;
    const vec3 gridCenter = gridOrigin + gridExtents * 0.5f;
    const auto gridTBox = Box<float>(gridOrigin - gridSpacing * 0.5f, gridOrigin + gridExtents + gridSpacing * 0.5f);
    const auto gridBBox = Box<float>(gridOrigin, gridOrigin + gridExtents);
    const dvec2 gridRange(dataOut->getScalarRange<float>());
    const float gridVoxelSize = length(gridSpacing) / std::sqrt(3.0f);

    dvec3 dGridOrigin(gridOrigin.x, gridOrigin.y, gridOrigin.z);
    dvec3 dGridSpacing(gridSpacing.x, gridSpacing.y, gridSpacing.z);
    dvec3 dGridExtents(gridExtents.x, gridExtents.y, gridExtents.z);

    // create a volume
    auto volume = std::make_shared<v3d::RegularGridVolumeGL>();
    volume->setMedium(dataOut, 0);
    volume->setParam("textureBox", gridTBox);
    volume->setParam("boundingBox", gridBBox);
    volume->setParam("clippingBox", gridBBox);
    volume->setParam("scalarMappingRange", gridRange);
    volume->setParam("sampleDistance", gridVoxelSize / 4.0f);
    volume->setParam("opacityUnitDistance", gridVoxelSize);
    volume->setParam("xSlicePosition", gridCenter.x);
    volume->setParam("ySlicePosition", gridCenter.y);
    volume->setParam("zSlicePosition", gridCenter.z);
    volume->commit();

    // setup the transfer function etc
    auto tfn = TransferFunction::fromRainbowMap();
    auto otf = std::unique_ptr<OcclusionTransferFunction>(new OcclusionTransferFunction());
    volume->setTransferFunction(std::move(tfn));
    volume->setTransferFunction2D(std::move(otf));

    // create toe scene
    auto scene = std::make_shared<RegularGridSceneGL>();
    scene->setVolume(volume);
    scene->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    scene->setTFPreIntegration(false);
    scene->setLighting(true);
    scene->setGlobalLighting(true);
    scene->lightSource()->setPosition(vec4(0.90037083625793457, 0.43512341380119324, 0, 0));
    scene->setEmptySpaceSkipping(false);
    auto camera = std::make_shared<Camera>();
    camera->lookAt(dGridOrigin + dGridExtents * dvec3(0.5, 0.5, 3.0),
                   dGridOrigin + dGridExtents * dvec3(0.5, 0.5, 0.5),
                   dvec3(0.0, 1.0, 0.0));
    double maxDim = max(max(dGridExtents.x, dGridExtents.y), dGridExtents.z);
    camera->perspective(45.0, camera->aspect(), maxDim * 0.01, maxDim * 10.0);
    scene->setCamera(camera);
    scene->setAllDirty(true);

    // create renderer
    auto renderer = std::make_shared<RegularGridPipelineGL>();
    renderer->setScene(scene);
    renderer->setFramebufferObject(fbo->sharedFramebufferObject());
    renderer->resize(quark::winW, quark::winH);

    // render
    glFinish();
    renderer->render();
    glFinish();

    // get framebuffer image
    std::vector<unsigned char> buffer(quark::winW * quark::winH * 4);
    GLuint currFbo = GLFramebufferObject::currentDrawBinding();
    fbo->bind();
    V3D_GL_PRINT_ERRORS();
    GLint readBuffer;
    glGetIntegerv(GL_READ_BUFFER, &readBuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, quark::winW, quark::winH, GL_BGRA, GL_UNSIGNED_BYTE, &buffer[0]);
    glReadBuffer(readBuffer);
    V3D_GL_PRINT_ERRORS();
    GLFramebufferObject::bind(currFbo);
    V3D_GL_PRINT_ERRORS();
    for (int i = 0; i < quark::winW * quark::winH; i++) buffer[i * 4 + 3] = 255;

    // save
    QImage img = QImage(&buffer[0], quark::winW, quark::winH, QImage::Format_RGB32).mirrored(false, true);
    img.save("image.PNG", 0, -1);
    std::cout << "save file as image.PNG" << std::endl;
}

}
