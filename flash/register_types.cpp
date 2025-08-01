// MIT License

// Copyright (c) 2021 Yakov Borevich, Funexpected LLC

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <core/object/class_db.h>
#include <core/config/project_settings.h>
#include "register_types.h"
#include "flash_player.h"
#include "flash_resources.h"
#ifdef MODULE_FLASH_WITH_ANIMATION_NODES
#include "animation_node_flash.h"
#endif

// 声明资源加载器变量
static Ref<ResourceFormatLoaderFlashDocument> resource_loader_flash_document;

#ifdef TOOLS_ENABLED
#include "core/config/engine.h"
#include "editor/export/editor_export.h"
#include "editor/editor_node.h"
#include "resource_importer_flash.h"

class EditorExportFlash : public EditorExportPlugin {
    GDCLASS(EditorExportFlash, EditorExportPlugin);

protected:
    static void _bind_methods() {}

public:
    virtual String get_name() const override {
        return "Flash";
    }

    virtual void _export_file(const String &p_path, const String &p_type, const HashSet<String> &p_features) override {
        // 导出Flash相关文件
        if (p_type == "FlashDocument") {
            // 添加.zfl文件到导出列表
            add_file(p_path, FileAccess::get_file_as_bytes(p_path), false);
            
            // 同时导出关联的纹理图集文件
            String base_path = p_path.get_basename();
            String extensions[] = {".ctexarray", ".s3tc.ctexarray", ".etc2.ctexarray", ".bptc.ctexarray", ".astc.ctexarray"};
            
            for (int i = 0; i < 5; i++) {
                String atlas_path = base_path + extensions[i];
                if (FileAccess::exists(atlas_path)) {
                    add_file(atlas_path, FileAccess::get_file_as_bytes(atlas_path), false);
                }
            }
        }
    }
};

static Ref<EditorExportFlash> editor_export_flash;

void _editor_init() {
    // 注册资源导入器
    Ref<ResourceImporterFlash> flash_importer;
    flash_importer.instantiate();
    ResourceFormatImporter::get_singleton()->add_importer(flash_importer);

    // 注册导出插件
    editor_export_flash.instantiate();
    EditorExport::get_singleton()->add_export_plugin(editor_export_flash);
}
#endif

void initialize_flash_module(ModuleInitializationLevel p_level) {
#ifdef TOOLS_ENABLED
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        EditorNode::add_init_callback(_editor_init);
        return;
    }
#endif

    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
	// core flash classes
	ClassDB::register_class<FlashPlayer>();
#ifdef MODULE_FLASH_WITH_ANIMATION_NODES
	ClassDB::register_class<FlashMachine>();
	ClassDB::register_class<AnimationNodeFlashClip>();
#endif

	// resources
	ClassDB::register_abstract_class<FlashElement>();
	ClassDB::register_class<FlashTextureRect>();
	ClassDB::register_class<FlashDocument>();
	ClassDB::register_class<FlashBitmapItem>();
	ClassDB::register_class<FlashTimeline>();
	ClassDB::register_class<FlashLayer>();
	ClassDB::register_class<FlashFrame>();
	ClassDB::register_class<FlashDrawing>();
	ClassDB::register_class<FlashInstance>();
	ClassDB::register_class<FlashGroup>();
	ClassDB::register_class<FlashShape>();
	ClassDB::register_class<FlashBitmapInstance>();
	ClassDB::register_class<FlashTween>();

// 创建并注册资源加载器
    resource_loader_flash_document.instantiate();
    ResourceLoader::add_resource_format_loader(resource_loader_flash_document);
	// loader
	// resource_loader_flash_texture.instantiate();
	// ResourceLoader::add_resource_format_loader(resource_loader_flash_texture);

#ifdef TOOLS_ENABLED
	EditorNode::add_init_callback(_editor_init);
#endif
}

void uninitialize_flash_module(ModuleInitializationLevel p_level) {
if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // 移除资源加载器
    if (resource_loader_flash_document.is_valid()) {
        ResourceLoader::remove_resource_format_loader(resource_loader_flash_document);
        resource_loader_flash_document.unref();
    }
	// ResourceLoader::remove_resource_format_loader(resource_loader_flash_texture);
	// resource_loader_flash_texture.unref();
}
