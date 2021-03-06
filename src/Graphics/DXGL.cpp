//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson), Min Shih                                //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "DXGL.h"

#include "Util/Library.h"
#include "Util/SourceCodeManager.h"

#include <QFile>
#include <QDataStream>

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
        v3d::SourceCodeManager::get().setSourceCode(fn.toStdString(), srcCode.toStdString());
    }
}

int v3d::dx::DXGL_create()
{
    loadModule();
    loadAllShaders();
    return 0;
}
