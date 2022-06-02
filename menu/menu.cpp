#include <ShlObj_core.h>
#include <unordered_map>
#include "menu.h"
#include "../menu/ImGui/code_editor.h"
#include "../constchars.h"
#include "../features/misc/logs.h"



#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar| ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)
#define NOALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)

std::vector <std::string> files;
std::vector <std::string> scripts;
std::string editing_script;

auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{ 713, 763 };

auto selected_script = 0;
auto loaded_editing_script = false;

static auto menu_setupped = false;
static auto should_update = true;

int weapon;
int tab;
int category;
int Rage_Tab;
int Legit_Tab;
int Visuals;
int Misc;
int Main;
int BStyleTab;
int Skins;

IDirect3DTexture9* all_skins[36];

std::string get_wep(int id, int custom_index = -1, bool knife = true)
{
	if (custom_index > -1)
	{
		if (knife)
		{
			switch (custom_index)
			{
			case 0: return crypt_str("weapon_knife");
			case 1: return crypt_str("weapon_bayonet");
			case 2: return crypt_str("weapon_knife_css");
			case 3: return crypt_str("weapon_knife_skeleton");
			case 4: return crypt_str("weapon_knife_outdoor");
			case 5: return crypt_str("weapon_knife_cord");
			case 6: return crypt_str("weapon_knife_canis");
			case 7: return crypt_str("weapon_knife_flip");
			case 8: return crypt_str("weapon_knife_gut");
			case 9: return crypt_str("weapon_knife_karambit");
			case 10: return crypt_str("weapon_knife_m9_bayonet");
			case 11: return crypt_str("weapon_knife_tactical");
			case 12: return crypt_str("weapon_knife_falchion");
			case 13: return crypt_str("weapon_knife_survival_bowie");
			case 14: return crypt_str("weapon_knife_butterfly");
			case 15: return crypt_str("weapon_knife_push");
			case 16: return crypt_str("weapon_knife_ursus");
			case 17: return crypt_str("weapon_knife_gypsy_jackknife");
			case 18: return crypt_str("weapon_knife_stiletto");
			case 19: return crypt_str("weapon_knife_widowmaker");
			}
		}
		else
		{
			switch (custom_index)
			{
			case 0: return crypt_str("ct_gloves");
			case 1: return crypt_str("studded_bloodhound_gloves");
			case 2: return crypt_str("t_gloves");
			case 3: return crypt_str("ct_gloves");
			case 4: return crypt_str("sporty_gloves");
			case 5: return crypt_str("slick_gloves");
			case 6: return crypt_str("leather_handwraps");
			case 7: return crypt_str("motorcycle_gloves");
			case 8: return crypt_str("specialist_gloves");
			case 9: return crypt_str("studded_hydra_gloves");
			}
		}
	}
	else
	{
		switch (id)
		{
		case 0: return crypt_str("knife");
		case 1: return crypt_str("gloves");
		case 2: return crypt_str("weapon_ak47");
		case 3: return crypt_str("weapon_aug");
		case 4: return crypt_str("weapon_awp");
		case 5: return crypt_str("weapon_cz75a");
		case 6: return crypt_str("weapon_deagle");
		case 7: return crypt_str("weapon_elite");
		case 8: return crypt_str("weapon_famas");
		case 9: return crypt_str("weapon_fiveseven");
		case 10: return crypt_str("weapon_g3sg1");
		case 11: return crypt_str("weapon_galilar");
		case 12: return crypt_str("weapon_glock");
		case 13: return crypt_str("weapon_m249");
		case 14: return crypt_str("weapon_m4a1_silencer");
		case 15: return crypt_str("weapon_m4a1");
		case 16: return crypt_str("weapon_mac10");
		case 17: return crypt_str("weapon_mag7");
		case 18: return crypt_str("weapon_mp5sd");
		case 19: return crypt_str("weapon_mp7");
		case 20: return crypt_str("weapon_mp9");
		case 21: return crypt_str("weapon_negev");
		case 22: return crypt_str("weapon_nova");
		case 23: return crypt_str("weapon_hkp2000");
		case 24: return crypt_str("weapon_p250");
		case 25: return crypt_str("weapon_p90");
		case 26: return crypt_str("weapon_bizon");
		case 27: return crypt_str("weapon_revolver");
		case 28: return crypt_str("weapon_sawedoff");
		case 29: return crypt_str("weapon_scar20");
		case 30: return crypt_str("weapon_ssg08");
		case 31: return crypt_str("weapon_sg556");
		case 32: return crypt_str("weapon_tec9");
		case 33: return crypt_str("weapon_ump45");
		case 34: return crypt_str("weapon_usp_silencer");
		case 35: return crypt_str("weapon_xm1014");
		default: return crypt_str("unknown");
		}
	}
}

IDirect3DTexture9* get_skin_preview(const char* weapon_name, const std::string& skin_name, IDirect3DDevice9* device)
{
	IDirect3DTexture9* skin_image = nullptr;
	std::string vpk_path;

	if (strcmp(weapon_name, crypt_str("unknown")) && strcmp(weapon_name, crypt_str("knife")) && strcmp(weapon_name, crypt_str("gloves")))
	{
		if (skin_name.empty() || skin_name == crypt_str("default"))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");
		else
			vpk_path = crypt_str("resource/flash/econ/default_generated/") + std::string(weapon_name) + crypt_str("_") + std::string(skin_name) + crypt_str("_light_large.png");
	}
	else
	{
		if (!strcmp(weapon_name, crypt_str("knife")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_knife.png");
		else if (!strcmp(weapon_name, crypt_str("gloves")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else if (!strcmp(weapon_name, crypt_str("unknown")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_snowball.png");
	}
	const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));
	if (handle)
	{
		int file_len = m_basefilesys()->Size(handle);
		char* image = new char[file_len];

		m_basefilesys()->Read(image, file_len, handle);
		m_basefilesys()->Close(handle);

		D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
		delete[] image;
	}

	if (!skin_image)
	{
		std::string vpk_path;

		if (strstr(weapon_name, crypt_str("bloodhound")) != NULL || strstr(weapon_name, crypt_str("hydra")) != NULL)
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");

		const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));

		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len];

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
			delete[] image;
		}
	}

	return skin_image;
}

void c_menu::menu_setup(ImGuiStyle& style)
{
	ImGui::StyleColorsClassic();
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
	ImGui::SetNextWindowBgAlpha(min(style.Alpha, 0.94f));

	styles.WindowPadding = style.WindowPadding;
	styles.WindowRounding = style.WindowRounding;
	styles.WindowMinSize = style.WindowMinSize;
	styles.ChildRounding = style.ChildRounding;
	styles.PopupRounding = style.PopupRounding;
	styles.FramePadding = style.FramePadding;
	styles.FrameRounding = style.FrameRounding;
	styles.ItemSpacing = style.ItemSpacing;
	styles.ItemInnerSpacing = style.ItemInnerSpacing;
	styles.TouchExtraPadding = style.TouchExtraPadding;
	styles.IndentSpacing = style.IndentSpacing;
	styles.ColumnsMinSpacing = style.ColumnsMinSpacing;
	styles.ScrollbarSize = style.ScrollbarSize;
	styles.ScrollbarRounding = style.ScrollbarRounding;
	styles.GrabMinSize = style.GrabMinSize;
	styles.GrabRounding = style.GrabRounding;
	styles.TabRounding = style.TabRounding;
	styles.TabMinWidthForUnselectedCloseButton = style.TabMinWidthForUnselectedCloseButton;
	styles.DisplayWindowPadding = style.DisplayWindowPadding;
	styles.DisplaySafeAreaPadding = style.DisplaySafeAreaPadding;
	styles.MouseCursorScale = style.MouseCursorScale;

	for (auto i = 0; i < g_cfg.skins.skinChanger.size(); i++)
		if (!all_skins[i])
			all_skins[i] = get_skin_preview(get_wep(i, (i == 0 || i == 1) ? g_cfg.skins.skinChanger.at(i).definition_override_vector_index : -1, i == 0).c_str(), g_cfg.skins.skinChanger.at(i).skin_name, device); //-V810

	menu_setupped = true;
}

void c_menu::dpi_resize(float scale_factor, ImGuiStyle& style)
{
	style.WindowPadding = (styles.WindowPadding * scale_factor);
	style.WindowRounding = (styles.WindowRounding * scale_factor);
	style.WindowMinSize = (styles.WindowMinSize * scale_factor);
	style.ChildRounding = (styles.ChildRounding * scale_factor);
	style.PopupRounding = (styles.PopupRounding * scale_factor);
	style.FramePadding = (styles.FramePadding * scale_factor);
	style.FrameRounding = (styles.FrameRounding * scale_factor);
	style.ItemSpacing = (styles.ItemSpacing * scale_factor);
	style.ItemInnerSpacing = (styles.ItemInnerSpacing * scale_factor);
	style.TouchExtraPadding = (styles.TouchExtraPadding * scale_factor);
	style.IndentSpacing = (styles.IndentSpacing * scale_factor);
	style.ColumnsMinSpacing = (styles.ColumnsMinSpacing * scale_factor);
	style.ScrollbarSize = (styles.ScrollbarSize * scale_factor);
	style.ScrollbarRounding = (styles.ScrollbarRounding * scale_factor);
	style.GrabMinSize = (styles.GrabMinSize * scale_factor);
	style.GrabRounding = (styles.GrabRounding * scale_factor);
	style.TabRounding = (styles.TabRounding * scale_factor);
	if (styles.TabMinWidthForUnselectedCloseButton != FLT_MAX)
		style.TabMinWidthForUnselectedCloseButton = (styles.TabMinWidthForUnselectedCloseButton * scale_factor);
	style.DisplayWindowPadding = (styles.DisplayWindowPadding * scale_factor);
	style.DisplaySafeAreaPadding = (styles.DisplaySafeAreaPadding * scale_factor);
	style.MouseCursorScale = (styles.MouseCursorScale * scale_factor);
}

void lua_loaded()
{
	c_lua::get().load_script(selected_script);
	c_lua::get().refresh_scripts();

	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}

	eventlogs::get().add(crypt_str("Loaded ") + scripts.at(selected_script) + crypt_str(" script"), false);
}

void Refresh_scripts()
{
	c_lua::get().refresh_scripts();
	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}
	eventlogs::get().add(crypt_str("Refresh scripts"), false);
}

void Reload_scripts()
{
	c_lua::get().reload_all_scripts();
	c_lua::get().refresh_scripts();

	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}
	eventlogs::get().add(crypt_str("Reload") + scripts.at(selected_script) + crypt_str(" script"), false);
}

void Unload_script()
{
	c_lua::get().unload_script(selected_script);
	c_lua::get().refresh_scripts();

	eventlogs::get().add(crypt_str("Unloaded ") + scripts.at(selected_script) + crypt_str(" script"), false);
}

void Open_lua()
{
	std::string folder = crypt_str("C:\\Oreo\\Scripts");
	ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

std::string get_config_dir()
{
	std::string folder;
	static TCHAR path[MAX_PATH];

	folder = crypt_str("C:\\Oreo\\");

	CreateDirectory(folder.c_str(), NULL);
	return folder;
}

void load_config()
{
	if (cfg_manager->files.empty())
		return;

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), false);

	for (auto i = 0; i < g_cfg.skins.skinChanger.size(); ++i)
		all_skins[i] = nullptr;

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), true);
	cfg_manager->config_files();
	eventlogs::get().add(crypt_str("Loaded ") + files.at(g_cfg.selected_config) + crypt_str(" config"), false);
}

void save_config()
{
	if (cfg_manager->files.empty())
		return;

	cfg_manager->save(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();
	eventlogs::get().add(crypt_str("Saved ") + files.at(g_cfg.selected_config) + crypt_str(" config"), false);
}

void remove_config()
{
	if (cfg_manager->files.empty())
		return;

	eventlogs::get().add(crypt_str("Removed ") + files.at(g_cfg.selected_config) + crypt_str(" config"), false);

	cfg_manager->remove(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();

	files = cfg_manager->files;

	for (auto& current : files)
		if (current.size() > 2)
			current.erase(current.size() - 3, 3);
}

void Open_configs()
{
	std::string folder = crypt_str("C:\\Oreo\\Configs");
	ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void add_config()
{
	auto empty = true;

	for (auto current : g_cfg.new_config_name)
	{
		if (current != ' ')
		{
			empty = false;
			break;
		}
	}

	if (empty)
		g_cfg.new_config_name = crypt_str("config");

	eventlogs::get().add(crypt_str("Added ") + g_cfg.new_config_name + crypt_str(" config"), false);

	if (g_cfg.new_config_name.find(crypt_str(".cfg")) == std::string::npos)
		g_cfg.new_config_name += crypt_str(".cfg");

	cfg_manager->save(g_cfg.new_config_name);
	cfg_manager->config_files();

	g_cfg.selected_config = cfg_manager->files.size() - 1; //-V103
	files = cfg_manager->files;

	for (auto& current : files)
		if (current.size() > 2)
			current.erase(current.size() - 4, 3);
}

bool LabelClick2(const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	char Buf[64];
	_snprintf(Buf, 62, crypt_str("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, crypt_str("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	*v = pressed;

	if (pressed || hovered)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(33 / 255.f, 33 / 255.f, 33 / 255.f, g_ctx.gui.pop_anim * 0.85f));
	if (label_size.x > 0.0f)

		ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::RenderText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), Buf);
	ImGui::PopFont();

	if (pressed || hovered)
		ImGui::PopStyleColor();

	return pressed;

}

bool draw_lua_button(const char* label, const char* label_id, bool load, bool save, int curr_config, bool create = false)
{
	bool pressed = false;
	ImGui::SetCursorPosX(8);
	if (ImGui::PlusButton(label, 0, ImVec2(240, 26), label_id, ImColor(25, 25, 25, 225), (25, 25, 25, 225)))
		selected_script = curr_config;

	static std::string edit2;
	static char config_name[36] = "\0";
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_PopupBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_PopupRounding, 4);
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(33 / 255.f, 33 / 255.f, 33 / 255.f, g_ctx.gui.pop_anim * 0.85f));
	if (ImGui::BeginPopup(label_id, ImGuiWindowFlags_NoMove))
	{
		ImGui::SetNextItemWidth(min(g_ctx.gui.pop_anim, 0.01f) * ImGui::GetFrameHeight() * 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_ctx.gui.pop_anim);
		auto clicked = false;
		
		bool one, ones, oness, two, open = false;
		if (!create)
		{
			if (LabelClick2(crypt_str("Load"), &one, label_id))
				lua_loaded();

			if (LabelClick2(crypt_str("Unload"), &two, label_id))
				Unload_script();

			if (LabelClick2(crypt_str("Reload"), &ones, label_id))
				Reload_scripts();
		}
		else
		{
			ImGui::SetCursorPosX(8);
			if (ImGui::CustomButton(crypt_str("Refresh"), crypt_str("##REFRESH_SCRIPTS"), ImVec2(193, 26 * 1)))
				Refresh_scripts();
			ImGui::SetCursorPosX(8);
			if (ImGui::CustomButton(crypt_str("Open lua folder"), crypt_str("##OPEN_LUA"), ImVec2(193, 26 * 1)))
				Open_lua();
		}
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(1);

	return pressed;
}

bool draw_config_button(const char* label, const char* label_id, bool load, bool save, int curr_config, bool create = false)
{
	bool pressed = false;
	ImGui::SetCursorPosX(8);
	if (ImGui::PlusButton(label, 0, ImVec2(240, 26), label_id, ImColor(25, 25, 25, 225), (25, 25, 25, 225)))g_cfg.selected_config = curr_config;

	static char config_name[36] = "\0";
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_PopupBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_PopupRounding, 4);
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(33 / 255.f, 33 / 255.f, 33 / 255.f, g_ctx.gui.pop_anim * 0.85f));
	if (ImGui::BeginPopup(label_id, ImGuiWindowFlags_NoMove))
	{
		ImGui::SetNextItemWidth(min(g_ctx.gui.pop_anim, 0.01f) * ImGui::GetFrameHeight() * 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_ctx.gui.pop_anim);
		auto clicked = false;
		bool one, ones, two, open = false;
		if (!create)
		{
			if (LabelClick2(crypt_str("Load"), &one, label_id))
				load_config();

			if (LabelClick2(crypt_str("Save"), &two, label_id))
				save_config();

			if (LabelClick2(crypt_str("Delete"), &ones, label_id))
				remove_config();

			if (LabelClick2(crypt_str("Open configs folder"), &open, label_id))
				Open_configs();
		}
		else
		{
			ImGui::SetCursorPosX(8);
			ImGui::Text("Input new config name");
			ImGui::SetCursorPosX(8);
			ImGui::PushItemWidth(193);
			ImGui::InputText(crypt_str("##configname"), config_name, sizeof(config_name));
			ImGui::PopItemWidth();

			ImGui::SetCursorPosX(8);
			auto new_label = std::string("Add " + std::string(config_name) + "");
			if (ImGui::CustomButton(new_label.c_str(), crypt_str("##CONFIG__CREATE"), ImVec2(193, 26 * 1)))
			{
				g_cfg.new_config_name = config_name;
				add_config();
			}
		}
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(1);
	return pressed;
}


__forceinline void padding(float x, float y)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x * c_menu::get().dpi_scale);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y * c_menu::get().dpi_scale);
}

void child_title(const char* label)
{
	ImGui::PushFont(c_menu::get().futura_large);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PopFont();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (12 * c_menu::get().dpi_scale));

	ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::Text(label);
	ImGui::PopFont();

	ImGui::PopStyleColor();
}

void draw_combo(const char* name, int& variable, const char* labels[], int count)
{
	ImGui::SetCursorPosX(13);
	ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::Text(name);
	ImGui::PopFont();

	ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::Combo(std::string(crypt_str("##COMBO__") + std::string(name)).c_str(), &variable, labels, count);
	ImGui::PopFont();
}

void draw_combo(const char* name, int& variable, bool (*items_getter)(void*, int, const char**), void* data, int count)
{
	ImGui::SetCursorPosX(13);
	ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::Text(name);
	ImGui::PopFont();

	ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::Combo(std::string(crypt_str("##COMBO__") + std::string(name)).c_str(), &variable, items_getter, data, count);
	ImGui::PopFont();
}

void draw_multicombo(std::string name, std::vector<int>& variable, const char* labels[], int count, std::string& preview)
{
	padding(-3, -6);
	ImGui::SetCursorPosX(13);

	ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::Text((crypt_str(" ") + name).c_str());
	ImGui::PopFont();

	padding(0, -5);

	auto hashname = crypt_str("##") + name;

	for (auto i = 0, j = 0; i < count; i++)
	{
		if (variable[i])
		{
			if (j)
				preview += crypt_str(", ") + (std::string)labels[i];
			else
				preview = labels[i];

			j++;
		}
	}
	ImGui::Spacing();

	ImGui::PushFont(c_menu::get().g_pMenuFont);
	if (ImGui::BeginCombo(hashname.c_str(), preview.c_str()))
	{
		ImGui::Spacing();
		ImGui::BeginGroup();
		{
			for (auto i = 0; i < count; i++)
				ImGui::Selectable(labels[i], (bool*)&variable[i], ImGuiSelectableFlags_DontClosePopups);
		}
		ImGui::EndGroup();
		ImGui::Spacing();

		ImGui::EndCombo();
	}
	ImGui::PopFont();

	preview = crypt_str("None");
}

bool LabelClick(const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	char Buf[64];
	_snprintf(Buf, 62, crypt_str("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, crypt_str("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	if (*v)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(25 / 255.f, 25 / 255.f, 25 / 225.f, 225.f));
	if (label_size.x > 0.0f)

		ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::RenderText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), Buf);
	ImGui::PopFont();

	if (*v)
		ImGui::PopStyleColor();

	return pressed;

}

void draw_keybind(const char* label, key_bind* key_bind, const char* unique_id)
{
	if (key_bind->key == KEY_ESCAPE)
		key_bind->key = KEY_NONE;

	auto clicked = false;
	auto text = (std::string)m_inputsys()->ButtonCodeToString(key_bind->key);

	if (key_bind->key <= KEY_NONE || key_bind->key >= KEY_MAX)
		text = crypt_str("None");

	if (hooks::input_shouldListen && hooks::input_receivedKeyval == &key_bind->key)
	{
		clicked = true;
		text = crypt_str("...");
	}

	auto textsize = ImGui::CalcTextSize(text.c_str()).x + 8 * c_menu::get().dpi_scale;
	auto labelsize = ImGui::CalcTextSize(label);


	ImGui::PushFont(c_menu::get().g_pMenuFont);
	ImGui::SetCursorPosX(13);
	ImGui::Text(label);
	ImGui::PopFont();

	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x - ImGui::CalcItemWidth()) - max(50 * c_menu::get().dpi_scale, textsize));
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3 * c_menu::get().dpi_scale);

	if (ImGui::KeybindButton(text.c_str(), unique_id, ImVec2(max(50 * c_menu::get().dpi_scale, textsize), 23 * c_menu::get().dpi_scale), clicked))
		clicked = true;


	if (clicked)
	{
		hooks::input_shouldListen = true;
		hooks::input_receivedKeyval = &key_bind->key;
	}

	static auto hold = false, toggle = false;

	switch (key_bind->mode)
	{
	case HOLD:
		hold = true;
		toggle = false;
		break;
	case TOGGLE:
		toggle = true;
		hold = false;
		break;
	}

	if (g_cfg.scripts.developer_mode && ImGui::IsItemHovered())
	{
		for (auto& item : cfg_manager->items)
		{
			if (key_bind == item->pointer)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
				ImGui::SetTooltip(item->name.c_str());
				ImGui::PopStyleVar();
				break;
			}
		}
	}

	if (ImGui::BeginPopup(unique_id))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6 * c_menu::get().dpi_scale);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Hold")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);



		ImGui::PushFont(c_menu::get().g_pMenuFont);
		if (LabelClick(crypt_str("Hold"), &hold, unique_id))
		{
			if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else
			{
				toggle = false;
				key_bind->mode = HOLD;
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::PopFont();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Toggle")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9 * c_menu::get().dpi_scale);

		ImGui::PushFont(c_menu::get().g_pMenuFont);
		if (LabelClick(crypt_str("Toggle"), &toggle, unique_id))
		{
			if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::PopFont();

		ImGui::EndPopup();
	}
}

void draw_semitabs(const char* labels[], int count, int& tab, ImGuiStyle& style)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (2 * c_menu::get().dpi_scale));

	float offset = 343 * c_menu::get().dpi_scale;

	for (int i = 0; i < count; i++)
		offset -= (ImGui::CalcTextSize(labels[i]).x) / 2 + style.FramePadding.x * 2;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	ImGui::BeginGroup();

	for (int i = 0; i < count; i++)
	{
		if (ImGui::ContentTab(labels[i], tab == i))
			tab = i;

		if (i + 1 != count)
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + style.ItemSpacing.x);
		}
	}

	ImGui::EndGroup();
}

__forceinline void tab_start()
{
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (20 * c_menu::get().dpi_scale), ImGui::GetCursorPosY() + (5 * c_menu::get().dpi_scale)));
}

__forceinline void tab_end()
{
	ImGui::PopStyleVar();
	ImGui::SetWindowFontScale(c_menu::get().dpi_scale);
}

void c_menu::rage()
{
	//weapon
	{
		ImGui::PushFont(weapon_icons);
		ImGui::SetCursorPosX(69);
		ImGui::SetCursorPosY(45);

		ImGui::BeginGroup();
		{
			if (ImGui::weapon("G", hooks::rage_weapon == 0, 47, 1, 1))hooks::rage_weapon = 0;//pistols
			ImGui::SameLine();
			if (ImGui::weapon("X", hooks::rage_weapon == 4, 49, 1, 1))hooks::rage_weapon = 4;//scar
			ImGui::SameLine();
			if (ImGui::weapon("a", hooks::rage_weapon == 5, 55, 1, 1))hooks::rage_weapon = 5;//ssg
			ImGui::SameLine();
			if (ImGui::weapon("Z", hooks::rage_weapon == 6, 57, 1, 1))hooks::rage_weapon = 6;//awp
			ImGui::SameLine();
		}
		ImGui::EndGroup();
		ImGui::PopFont();
	}

	if (Rage_Tab == 0)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Main", ImVec2(334, 360));
		{
			ImGui::Checkbox(crypt_str("Enabled"), &g_cfg.ragebot.enable);

			if (g_cfg.ragebot.enable)
				g_cfg.legitbot.enabled = false;
			g_cfg.ragebot.zeus_bot = true;
			g_cfg.ragebot.knife_bot = true;

			ImGui::SetCursorPosX(1);
			ImGui::SliderInt(crypt_str("Field of view"), &g_cfg.ragebot.field_of_view, 1, 180, false, crypt_str("%d°"));

			ImGui::Checkbox(crypt_str("Silent aim"), &g_cfg.ragebot.silent_aim);
			ImGui::Checkbox(crypt_str("Compensate recoil"), &g_cfg.ragebot.remove_recoil);
			ImGui::Checkbox(crypt_str("Automatic awall"), &g_cfg.ragebot.autowall);
			ImGui::Checkbox(crypt_str("Automatic fire"), &g_cfg.ragebot.autoshoot);
			ImGui::Checkbox(crypt_str("Automatic scope"), &g_cfg.ragebot.autoscope);
			ImGui::Checkbox(crypt_str("Desync correction"), &g_cfg.ragebot.desync_correction);
			if (g_cfg.ragebot.desync_correction)
				draw_keybind(crypt_str("Resolver roll"), &g_cfg.ragebot.desync_roll_resolver, crypt_str("##HOTKEY_ROLL"));
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(25, 540));
		ImGui::MenuChild("Help", ImVec2(334, 115));
		{
			ImGui::Checkbox(crypt_str("Defensive doubletap"), &g_cfg.ragebot.defensive_doubletap);
			ImGui::Checkbox(crypt_str("Anti-defensive (test)"), &g_cfg.ragebot.anti_defensive);
			ImGui::Checkbox(crypt_str("Adaptive two shot damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].adaptive_two_shot);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("General", ImVec2(316, 360));
		{
			draw_multicombo(crypt_str("Hitboxes"), g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes, hitboxes, ARRAYSIZE(hitboxes), preview);

			ImGui::Checkbox(crypt_str("Hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance);
			if (g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance)
				ImGui::SliderInt(crypt_str("Hitchance amount"), &g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance_amount, 1, 100);

			if (g_cfg.ragebot.double_tap && hooks::rage_weapon <= 4)
			{
				ImGui::Checkbox(crypt_str("Double tap hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance);
				if (g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance)
					ImGui::SliderInt(crypt_str("Double tap hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance_amount, 1, 100);
			}

			draw_keybind(crypt_str("Damage override"), &g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key, crypt_str("##HOTKEY__DAMAGE_OVERRIDE"));
			if (g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key > KEY_NONE && g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key < KEY_MAX)
				ImGui::SliderInt(crypt_str("Minimum override damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_override_damage, 1, 120, true);

			ImGui::SliderInt(crypt_str("Minimum damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_visible_damage, 1, 120, true);
			if (g_cfg.ragebot.autowall)
				ImGui::SliderInt(crypt_str("Minimum wall damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_damage, 1, 120, true);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 540));
		ImGui::MenuChild("Exploit", ImVec2(316, 115));
		{
			ImGui::Checkbox(crypt_str("Double tap"), &g_cfg.ragebot.double_tap);
			if (g_cfg.ragebot.double_tap)
			{
				ImGui::SameLine();
				draw_keybind(crypt_str(""), &g_cfg.ragebot.double_tap_key, crypt_str("##HOTKEY_DT"));
			}

			ImGui::Checkbox(crypt_str("Hide shots"), &g_cfg.antiaim.hide_shots);
			if (g_cfg.antiaim.hide_shots)
			{
				ImGui::SameLine();
				draw_keybind(crypt_str(""), &g_cfg.antiaim.hide_shots_key, crypt_str("##HOTKEY_HIDESHOTS"));
			}
		}
		ImGui::EndChild();
	}

	if (Rage_Tab == 1)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Main", ImVec2(334, 360));
		{
			draw_combo(crypt_str("Target selection"), g_cfg.ragebot.weapon[hooks::rage_weapon].selection_type, selection, ARRAYSIZE(selection));
			padding(0, 3);

			if (g_cfg.ragebot.weapon[hooks::rage_weapon].adaptive_point_scale == true == false)
			{
				ImGui::Checkbox(crypt_str("Static point scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale);
				if (g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale)
				{
					ImGui::SliderFloat(crypt_str("Head scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale, 0.0f, 1.0f, g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale ? crypt_str("%.2f") : crypt_str("None"));
					ImGui::SliderFloat(crypt_str("Body scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale, 0.0f, 1.0f, g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale ? crypt_str("%.2f") : crypt_str("None"));
				}
			}

			ImGui::Checkbox(crypt_str("Adaptive point scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].adaptive_point_scale);

			ImGui::Checkbox(crypt_str("Enable max misses"), &g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses);
			if (g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses)
				ImGui::SliderInt(crypt_str("Max misses"), &g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses_amount, 0, 6);

			ImGui::Checkbox(crypt_str("Ignore limbs when moving"), &g_cfg.ragebot.weapon[hooks::rage_weapon].rage_aimbot_ignore_limbs);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(25, 540));
		ImGui::MenuChild("Other", ImVec2(334, 115));
		{
			draw_keybind(crypt_str("Force safe points"), &g_cfg.ragebot.safe_point_key, crypt_str("##HOKEY_FORCE_SAFE_POINTS"));
			ImGui::Spacing();
			draw_keybind(crypt_str("Force body aim"), &g_cfg.ragebot.body_aim_key, crypt_str("##HOKEY_FORCE_BODY_AIM"));
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("Anti Aim", ImVec2(316, 360));
		{
			static auto type = 0;

			ImGui::Checkbox(crypt_str("Enable"), &g_cfg.antiaim.enable);
			draw_combo(crypt_str("Anti-aim type"), g_cfg.antiaim.antiaim_type, antiaim_type, ARRAYSIZE(antiaim_type));

			if (g_cfg.antiaim.antiaim_type)
			{
				padding(0, 3);
				draw_combo(crypt_str("Desync"), g_cfg.antiaim.desync, desync, ARRAYSIZE(desync));

				if (g_cfg.antiaim.desync)
				{
					padding(0, 3);
					draw_combo(crypt_str("LBY type"), g_cfg.antiaim.legit_lby_type, lby_type, ARRAYSIZE(lby_type));

					if (g_cfg.antiaim.desync == 1)
					{
						draw_keybind("Invert desync", &g_cfg.antiaim.flip_desync, "##HOTKEY_INVERT_DESYNC");
					}
				}
			}
			else
			{
				draw_combo(crypt_str("Pitch"), g_cfg.antiaim.type[type].pitch, pitch, ARRAYSIZE(pitch));
				padding(0, 3);
				draw_combo(crypt_str("Yaw"), g_cfg.antiaim.type[type].yaw, yaw, ARRAYSIZE(yaw));
				padding(0, 3);
				draw_combo(crypt_str("Base angle"), g_cfg.antiaim.type[type].base_angle, baseangle, ARRAYSIZE(baseangle));

				if (g_cfg.antiaim.type[type].yaw)
				{
					ImGui::SliderInt(g_cfg.antiaim.type[type].yaw == 1 ? crypt_str("Jitter range") : crypt_str("Spin range"), &g_cfg.antiaim.type[type].range, 1, 180);

					if (g_cfg.antiaim.type[type].yaw == 2)
						ImGui::SliderInt(crypt_str("Spin speed"), &g_cfg.antiaim.type[type].speed, 1, 15);

					padding(0, 3);
				}

				draw_combo(crypt_str("Desync"), g_cfg.antiaim.type[type].desync, desync, ARRAYSIZE(desync));

				if (g_cfg.antiaim.type[type].desync)
				{
					if (type == ANTIAIM_STAND)
					{
						padding(0, 3);
						draw_combo(crypt_str("LBY type"), g_cfg.antiaim.lby_type, lby_type, ARRAYSIZE(lby_type));
					}

					if (type != ANTIAIM_STAND || !g_cfg.antiaim.lby_type)
					{
						ImGui::SliderInt(crypt_str("Desync range"), &g_cfg.antiaim.type[type].desync_range, 1, 100, "%.0f%%");
						ImGui::SliderInt(crypt_str("Body lean"), &g_cfg.antiaim.type[type].body_lean, 0, 100);
						ImGui::SliderInt(crypt_str("Inverted body lean"), &g_cfg.antiaim.type[type].inverted_body_lean, 0, 100);
					}
				}

				draw_keybind("Invert desync", &g_cfg.antiaim.flip_desync, "##HOTKEY_INVERT_DESYNC");

				draw_keybind("Manual back", &g_cfg.antiaim.manual_back, "##HOTKEY_INVERT_BACK");

				draw_keybind("Manual left", &g_cfg.antiaim.manual_left, "##HOTKEY_INVERT_LEFT");

				draw_keybind("Manual right", &g_cfg.antiaim.manual_right, "##HOTKEY_INVERT_RIGHT");

				if (g_cfg.antiaim.manual_back.key > KEY_NONE && g_cfg.antiaim.manual_back.key < KEY_MAX || g_cfg.antiaim.manual_left.key > KEY_NONE && g_cfg.antiaim.manual_left.key < KEY_MAX || g_cfg.antiaim.manual_right.key > KEY_NONE && g_cfg.antiaim.manual_right.key < KEY_MAX)
				{
					ImGui::Checkbox(crypt_str("Manuals indicator"), &g_cfg.antiaim.flip_indicator);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##invc"), &g_cfg.antiaim.flip_indicator_color, ALPHA);
				}
			}

		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 540));
		ImGui::MenuChild("Accuracy", ImVec2(316, 115));
		{
			ImGui::Checkbox(crypt_str("Automatic stop"), &g_cfg.ragebot.weapon[hooks::rage_weapon].autostop);
			if (g_cfg.ragebot.weapon[hooks::rage_weapon].autostop)
				draw_multicombo(crypt_str("Modifiers"), g_cfg.ragebot.weapon[hooks::rage_weapon].autostop_modifiers, autostop_modifiers, ARRAYSIZE(autostop_modifiers), preview);
		}
		ImGui::EndChild();
	}
}

void c_menu::legit()
{
	{
		ImGui::PushFont(weapon_icons);
		ImGui::SetCursorPos(ImVec2(40, 45));

		ImGui::BeginGroup();
		{
			if (ImGui::weapon("G", hooks::legit_weapon == 0, 58, 1, 1))hooks::legit_weapon = 0; ImGui::SameLine();//pistol  
			if (ImGui::weapon("S", hooks::legit_weapon == 2, 58, 1, 1))hooks::legit_weapon = 2; ImGui::SameLine();//rifle   
			if (ImGui::weapon("L", hooks::legit_weapon == 3, 58, 1, 1))hooks::legit_weapon = 3; ImGui::SameLine();//smg  
			if (ImGui::weapon("a", hooks::legit_weapon == 4, 58, 1, 1))hooks::legit_weapon = 4; ImGui::SameLine();//ssg
			if (ImGui::weapon("d", hooks::legit_weapon == 5, 57, 1, 1))hooks::legit_weapon = 5; ImGui::SameLine();//heavy   
		}
		ImGui::EndGroup();
		ImGui::PopFont();
	}

	if (Legit_Tab == 0)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Main", ImVec2(334, 570));
		{
			ImGui::Checkbox(crypt_str("Enable"), &g_cfg.legitbot.enabled);
			ImGui::SameLine();
			draw_keybind(crypt_str(""), &g_cfg.legitbot.key, crypt_str("##HOTKEY_LGBT_KEY"));
			if (g_cfg.legitbot.enabled)
				g_cfg.ragebot.enable = false;

			ImGui::Checkbox(crypt_str("Friendly fire"), &g_cfg.legitbot.friendly_fire);
			ImGui::Checkbox(crypt_str("Automatic pistols"), &g_cfg.legitbot.autopistol);

			ImGui::Checkbox(crypt_str("Automatic scope"), &g_cfg.legitbot.autoscope);

			if (g_cfg.legitbot.autoscope)
				ImGui::Checkbox(crypt_str("Unscope after shot"), &g_cfg.legitbot.unscope);

			ImGui::Checkbox(crypt_str("Snipers in zoom only"), &g_cfg.legitbot.sniper_in_zoom_only);

			ImGui::Checkbox(crypt_str("Aim if in air"), &g_cfg.legitbot.do_if_local_in_air);
			ImGui::Checkbox(crypt_str("Aim if flashed"), &g_cfg.legitbot.do_if_local_flashed);
			ImGui::Checkbox(crypt_str("Aim thru smoke"), &g_cfg.legitbot.do_if_enemy_in_smoke);

			draw_keybind(crypt_str("Automatic fire key"), &g_cfg.legitbot.autofire_key, crypt_str("##HOTKEY_AUTOFIRE_LGBT_KEY"));
			ImGui::SliderInt(crypt_str("Automatic fire delay"), &g_cfg.legitbot.autofire_delay, 0, 12, false, (!g_cfg.legitbot.autofire_delay ? crypt_str("None") : (g_cfg.legitbot.autofire_delay == 1 ? crypt_str("%d tick") : crypt_str("%d ticks"))));

			draw_combo(crypt_str("RCS type"), g_cfg.legitbot.weapon[hooks::legit_weapon].rcs_type, RCSType, ARRAYSIZE(RCSType));
			ImGui::SliderFloat(crypt_str("RCS amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].rcs, 0.f, 100.f, crypt_str("%.0f%%"), 1.f);

			if (g_cfg.legitbot.weapon[hooks::legit_weapon].rcs > 0)
			{
				ImGui::SliderFloat(crypt_str("RCS custom FOV"), &g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov ? crypt_str("None") : crypt_str("%.2f"))); //-V550
				ImGui::SliderFloat(crypt_str("RCS Custom smooth"), &g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth, 0.f, 12.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth ? crypt_str("None") : crypt_str("%.1f"))); //-V550
			}
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("Settings", ImVec2(316, 570));
		{
			const char* hitbox_legit[3] = { crypt_str("Closest"), crypt_str("Head"), crypt_str("Body") };
			draw_combo(crypt_str("Hitbox"), g_cfg.legitbot.weapon[hooks::legit_weapon].priority, hitbox_legit, ARRAYSIZE(hitbox_legit));
			ImGui::SliderFloat(crypt_str("Maximum FOV amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].fov, 0.f, 30.f, crypt_str("%.2f"));
			ImGui::Spacing();
			ImGui::SliderFloat(crypt_str("Silent FOV"), &g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov ? crypt_str("None") : crypt_str("%.2f"))); //-V550

			ImGui::Spacing();

			draw_combo(crypt_str("Smooth type"), g_cfg.legitbot.weapon[hooks::legit_weapon].smooth_type, LegitSmooth, ARRAYSIZE(LegitSmooth));
			ImGui::SliderFloat(crypt_str("Smooth amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].smooth, 1.f, 12.f, crypt_str("%.1f"));

			ImGui::Spacing();

			ImGui::SliderInt(crypt_str("Automatic wall damage"), &g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg, 0, 100, false, (!g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg ? crypt_str("None") : crypt_str("%d")));
			ImGui::SliderInt(crypt_str("Automatic fire hitchance"), &g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance, 0, 100, false, (!g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance ? crypt_str("None") : crypt_str("%d")));
			ImGui::SliderFloat(crypt_str("Target switch delay"), &g_cfg.legitbot.weapon[hooks::legit_weapon].target_switch_delay, 0.f, 1.f);
			ImGui::Checkbox(crypt_str("Automatic stop"), &g_cfg.legitbot.weapon[hooks::legit_weapon].auto_stop);
		}
		ImGui::EndChild();
	}
}

void c_menu::visuals()
{
	if (Visuals == 0)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Main", ImVec2(334, 570));
		{
			ImGui::Checkbox(crypt_str("Enabled"), &g_cfg.player.enable);

			if (category == ENEMY)
			{
				ImGui::Checkbox(crypt_str("OOF arrows"), &g_cfg.player.arrows);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##arrowscolor"), &g_cfg.player.arrows_color, ALPHA);

				if (g_cfg.player.arrows)
				{
					ImGui::SliderInt(crypt_str("Arrows distance"), &g_cfg.player.distance, 0, 100);
					ImGui::SliderInt(crypt_str("Arrows size"), &g_cfg.player.size, 0, 100);
				}
			}

			ImGui::Checkbox(crypt_str("Bounding box"), &g_cfg.player.type[category].box);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##boxcolor"), &g_cfg.player.type[category].box_color, ALPHA);

			ImGui::Checkbox(crypt_str("Name"), &g_cfg.player.type[category].name);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##namecolor"), &g_cfg.player.type[category].name_color, ALPHA);

			ImGui::Checkbox(crypt_str("Health bar"), &g_cfg.player.type[category].health);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##healthcolor"), &g_cfg.player.type[category].health_color, ALPHA);

			for (auto i = 0, j = 0; i < ARRAYSIZE(flags); i++)
			{
				if (g_cfg.player.type[category].flags[i])
				{
					if (j)
						preview += crypt_str(", ") + (std::string)flags[i];
					else
						preview = flags[i];

					j++;
				}
			}

			draw_multicombo(crypt_str("Flags"), g_cfg.player.type[category].flags, flags, ARRAYSIZE(flags), preview);
			draw_multicombo(crypt_str("Weapon"), g_cfg.player.type[category].weapon, weaponplayer, ARRAYSIZE(weaponplayer), preview);

			if (g_cfg.player.type[category].weapon[WEAPON_ICON] || g_cfg.player.type[category].weapon[WEAPON_TEXT])
			{
				ImGui::PushFont(c_menu::get().g_pMenuFont);
				ImGui::SetCursorPosX(9);
				ImGui::Text(crypt_str("Color "));
				ImGui::PopFont();

				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##weapcolor"), &g_cfg.player.type[category].weapon_color, ALPHA);
			}

			ImGui::Checkbox(crypt_str("Ammo bar"), &g_cfg.player.type[category].ammo);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##ammocolor"), &g_cfg.player.type[category].ammobar_color, ALPHA);

			ImGui::Checkbox(crypt_str("Footsteps"), &g_cfg.player.type[category].footsteps);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##footstepscolor"), &g_cfg.player.type[category].footsteps_color, ALPHA);

			if (g_cfg.player.type[category].footsteps)
			{
				ImGui::SliderInt(crypt_str("Thickness"), &g_cfg.player.type[category].thickness, 0, 10);
				ImGui::SliderInt(crypt_str("Radius"), &g_cfg.player.type[category].radius, 50, 500);
			}

			if (category == ENEMY || category == TEAM)
			{
				ImGui::Checkbox(crypt_str("Snap lines"), &g_cfg.player.type[category].snap_lines);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##snapcolor"), &g_cfg.player.type[category].snap_lines_color, ALPHA);
			}

		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("General", ImVec2(316, 570));
		{
			if (category == LOCAL)
				draw_combo(crypt_str("Type"), g_cfg.player.local_chams_type, local_chams_type, ARRAYSIZE(local_chams_type));

			if (category != LOCAL || !g_cfg.player.local_chams_type)
				draw_multicombo(crypt_str("Chams"), g_cfg.player.type[category].chams, g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE] ? chamsvisact : chamsvis, g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE] ? ARRAYSIZE(chamsvisact) : ARRAYSIZE(chamsvis), preview);

			if (g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE] || category == LOCAL && g_cfg.player.local_chams_type)
			{
				if (category == LOCAL && g_cfg.player.local_chams_type)
				{
					ImGui::Checkbox(crypt_str("Enable desync chams"), &g_cfg.player.fake_chams_enable);
					ImGui::Checkbox(crypt_str("Visualize fake-lag"), &g_cfg.player.visualize_lag);
					ImGui::Checkbox(crypt_str("Layered"), &g_cfg.player.layered);

					draw_combo(crypt_str("Player chams material"), g_cfg.player.fake_chams_type, chamstype, ARRAYSIZE(chamstype));

					ImGui::Text(crypt_str("Color "));
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##fakechamscolor"), &g_cfg.player.fake_chams_color, ALPHA);

					if (g_cfg.player.fake_chams_type != 6)
					{
						ImGui::Checkbox(crypt_str("Double material "), &g_cfg.player.fake_double_material);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##doublecolormat"), &g_cfg.player.fake_double_material_color, ALPHA);
					}

					ImGui::Checkbox(crypt_str("Transparency in scope"), &g_cfg.player.transparency_in_scope);

					if (g_cfg.player.transparency_in_scope)
						ImGui::SliderFloat(crypt_str("Alpha"), &g_cfg.player.transparency_in_scope_amount, 0.0f, 1.0f);
				}
				else
				{
					draw_combo(crypt_str("Player chams material"), g_cfg.player.type[category].chams_type, chamstype, ARRAYSIZE(chamstype));

					if (g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE])
					{
						ImGui::PushFont(c_menu::get().g_pMenuFont);
						ImGui::SetCursorPosX(9);
						ImGui::Text(crypt_str("Visible "));
						ImGui::PopFont();

						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##chamsvisible"), &g_cfg.player.type[category].chams_color, ALPHA);
					}

					if (g_cfg.player.type[category].chams[PLAYER_CHAMS_VISIBLE] && g_cfg.player.type[category].chams[PLAYER_CHAMS_INVISIBLE])
					{
						ImGui::PushFont(c_menu::get().g_pMenuFont);
						ImGui::SetCursorPosX(9);
						ImGui::Text(crypt_str("Invisible "));;
						ImGui::PopFont();

						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##chamsinvisible"), &g_cfg.player.type[category].xqz_color, ALPHA);
					}

					if (category == ENEMY)
					{
						if (category == 0 || ENEMY)
						{
							ImGui::Checkbox(crypt_str("Backtrack chams"), &g_cfg.player.backtrack_chams);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##backtrackcolor"), &g_cfg.player.backtrack_chams_color, ALPHA);

							if (g_cfg.player.backtrack_chams)
								draw_combo(crypt_str("Backtrack chams material"), g_cfg.player.backtrack_chams_material, chamstype, ARRAYSIZE(chamstype));
						}

						if (category == 0 || ENEMY || category == 1 || TEAM)
						{
							ImGui::Checkbox(crypt_str("Ragdoll chams"), &g_cfg.player.type[category].ragdoll_chams);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##ragdollcolor"), &g_cfg.player.type[category].ragdoll_chams_color, ALPHA);

							if (g_cfg.player.type[category].ragdoll_chams)
								draw_combo(crypt_str("Ragdoll chams material"), g_cfg.player.type[category].ragdoll_chams_material, chamstype, ARRAYSIZE(chamstype));
						}

						if (category == 0 || ENEMY || category == 1 || TEAM || category == 2 || LOCAL)
						{
							ImGui::Checkbox(crypt_str("Glow"), &g_cfg.player.type[category].glow);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##glowcolor"), &g_cfg.player.type[category].glow_color, ALPHA);

							if (g_cfg.player.type[category].glow)
								draw_combo(crypt_str("Glow type"), g_cfg.player.type[category].glow_type, glowtype, ARRAYSIZE(glowtype));
						}

						if (category == 0 || ENEMY)
						{
							ImGui::Checkbox(crypt_str("On shot chams"), &g_cfg.player.on_shot_chams);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##lagcompcolor"), &g_cfg.player.on_shot_chams_color, ALPHA);

							if (g_cfg.player.on_shot_chams)
								draw_combo(("Aimbot hitboxes type"), g_cfg.player.on_shot_chams_type, on_shot_chams_type, ARRAYSIZE(on_shot_chams_type));
						}
					}
				}
			}
		}
		ImGui::EndChild();
	}

	if (Visuals == 1)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Grenade", ImVec2(334, 150));
		{
			ImGui::Checkbox(crypt_str("Grenade prediction"), &g_cfg.esp.grenade_prediction);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##grenpredcolor"), &g_cfg.esp.grenade_prediction_color, ALPHA);

			if (g_cfg.esp.grenade_prediction)
			{
				ImGui::Checkbox(crypt_str("On click"), &g_cfg.esp.on_click);
				ImGui::PushFont(c_menu::get().g_pMenuFont);
				ImGui::SetCursorPosX(9);
				ImGui::Text(crypt_str("Tracer color "));
				ImGui::PopFont();

				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##tracergrenpredcolor"), &g_cfg.esp.grenade_prediction_tracer_color, ALPHA);
			}

			ImGui::Checkbox(crypt_str("Grenade proximity warning"), &g_cfg.esp.grenade_proximity_warning);

			if (g_cfg.esp.grenade_proximity_warning)
			{
				ImGui::PushFont(c_menu::get().g_pMenuFont);
				ImGui::SetCursorPosX(9);
				ImGui::Text(crypt_str("Warning color"));
				ImGui::PopFont();

				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##grenade_color512342"), &g_cfg.esp.grenade_proximity_warning_progress_color, ALPHA);

				ImGui::Checkbox(crypt_str("Offscreen warning"), &g_cfg.esp.offscreen_proximity);
			}
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(25, 320));
		ImGui::MenuChild("Timer", ImVec2(334, 70));
		{
			ImGui::Checkbox(crypt_str("Molotov timer"), &g_cfg.esp.molotov_timer);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##molotovtimer"), &g_cfg.esp.molotov_timer_color, ALPHA);

			ImGui::Checkbox(crypt_str("Smoke timer"), &g_cfg.esp.smoke_timer);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##smoketimer"), &g_cfg.esp.smoke_timer_color, ALPHA);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("General", ImVec2(316, 260));
		{
			ImGui::Checkbox(crypt_str("Enable"), &g_cfg.player.enable);

			ImGui::Checkbox(crypt_str("Rain"), &g_cfg.esp.rain);
			ImGui::Checkbox(crypt_str("Gravity ragdolls"), &g_cfg.misc.ragdolls);
			ImGui::Checkbox(crypt_str("Full bright"), &g_cfg.esp.bright);

			draw_combo(crypt_str("Skybox"), g_cfg.esp.skybox, skybox, ARRAYSIZE(skybox));

			ImGui::PushFont(c_menu::get().g_pMenuFont);
			ImGui::SetCursorPosX(9);
			ImGui::Text(crypt_str("Color"));
			ImGui::PopFont();

			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##skyboxcolor"), &g_cfg.esp.skybox_color, NOALPHA);

			ImGui::Checkbox(crypt_str("Color modulation"), &g_cfg.esp.nightmode);

			if (g_cfg.esp.nightmode)
			{
				ImGui::PushFont(c_menu::get().g_pMenuFont);
				ImGui::SetCursorPosX(9);
				ImGui::Text(crypt_str("World color "));
				ImGui::PopFont();

				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##worldcolor"), &g_cfg.esp.world_color, ALPHA);

				ImGui::PushFont(c_menu::get().g_pMenuFont);
				ImGui::SetCursorPosX(9);
				ImGui::Text(crypt_str("Props color "));
				ImGui::PopFont();

				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##propscolor"), &g_cfg.esp.props_color, ALPHA);
			}
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(25, 430));
		ImGui::MenuChild("Tracer", ImVec2(334, 115));
		{
			ImGui::Checkbox(crypt_str("Client bullet impacts"), &g_cfg.esp.client_bullet_impacts);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##clientbulletimpacts"), &g_cfg.esp.client_bullet_impacts_color, ALPHA);

			ImGui::Checkbox(crypt_str("Server bullet impacts"), &g_cfg.esp.server_bullet_impacts);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##serverbulletimpacts"), &g_cfg.esp.server_bullet_impacts_color, ALPHA);

			ImGui::Checkbox(crypt_str("Local bullet tracers"), &g_cfg.esp.bullet_tracer);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##bulltracecolor"), &g_cfg.esp.bullet_tracer_color, ALPHA);

			ImGui::Checkbox(crypt_str("Enemy bullet tracers"), &g_cfg.esp.enemy_bullet_tracer);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##enemybulltracecolor"), &g_cfg.esp.enemy_bullet_tracer_color, ALPHA);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 430));
		ImGui::MenuChild("Trail", ImVec2(316, 115));
		{
			ImGui::PushFont(c_menu::get().g_pMenuFont);
			ImGui::SetCursorPosX(9);
			ImGui::Combo("Trail type", &g_cfg.misc.trail_types, "None\0Particles\0Line\0");
			ImGui::PopFont();

			if (g_cfg.misc.trail_types == 1)
				ImGui::Checkbox(crypt_str("Movement trail"), &g_cfg.misc.movement_trails);

			if (g_cfg.misc.trail_types == 2)
			{
				ImGui::Checkbox(crypt_str("Movement trail"), &g_cfg.misc.movement_trails);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##movement_trail"), &g_cfg.misc.trail_color, ALPHA);

				ImGui::Checkbox(crypt_str("Rainbow trail"), &g_cfg.misc.rainbow_trails);
			}
		}
		ImGui::EndChild();
	}

	if (Visuals == 2)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Viewmodel", ImVec2(334, 220));
		{
			ImGui::Checkbox("Viewmodel in scope", &g_cfg.esp.viewmodel_in_scope);

			ImGui::Checkbox(crypt_str("Rare animations"), &g_cfg.skins.rare_animations);
			ImGui::SliderInt(crypt_str("Viewmodel field of view"), &g_cfg.esp.viewmodel_fov, 0, 100);
			ImGui::SliderInt(crypt_str("Viewmodel X"), &g_cfg.esp.viewmodel_x, -50, 50);
			ImGui::SliderInt(crypt_str("Viewmodel Y"), &g_cfg.esp.viewmodel_y, -50, 50);
			ImGui::SliderInt(crypt_str("Viewmodel Z"), &g_cfg.esp.viewmodel_z, -50, 50);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("Render", ImVec2(316, 220));
		{
			draw_multicombo(crypt_str("Removals"), g_cfg.esp.removals, removals, ARRAYSIZE(removals), preview);

			if (g_cfg.esp.removals[REMOVALS_SCOPE])
			{
				ImGui::Text("  Scope color");
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##removals_scope_color"), &g_cfg.esp.removals_scope_color, ALPHA);
				ImGui::SliderInt(crypt_str("Scope speed"), &g_cfg.esp.removals_scope_speed, 1, 10);
				ImGui::SliderInt(crypt_str("Scope distance"), &g_cfg.esp.removals_scope_distance, 1, 20);
				ImGui::SliderInt(crypt_str("Scope length"), &g_cfg.esp.removals_scope_length, 1, 20);
			}

			if (g_cfg.esp.removals[REMOVALS_ZOOM])
				ImGui::Checkbox(crypt_str("Fix zoom sensivity"), &g_cfg.esp.fix_zoom_sensivity);

			ImGui::Checkbox(crypt_str("Disable panorama blur"), &g_cfg.esp.disablepanoramablur);
			ImGui::Checkbox(crypt_str("Bomb indicator"), &g_cfg.esp.bomb_timer);

			ImGui::Checkbox(crypt_str("Damage marker"), &g_cfg.esp.damage_marker);
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##Damagemarkercolor"), &g_cfg.esp.damage_marker_color, ALPHA);

			ImGui::Checkbox(crypt_str("Sniper crosshair "), &g_cfg.esp.sniper_crosshair);
			ImGui::Checkbox(crypt_str("Penetration crosshair"), &g_cfg.esp.penetration_reticle);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(25, 390));
		ImGui::MenuChild("Fog", ImVec2(334, 150));
		{
			ImGui::Checkbox(crypt_str("Fog modulation"), &g_cfg.esp.fog);
			if (g_cfg.esp.fog)
			{
				ImGui::SliderInt(crypt_str("Distance"), &g_cfg.esp.fog_distance, 0, 2500);
				ImGui::SliderInt(crypt_str("Density"), &g_cfg.esp.fog_density, 0, 100);

				ImGui::PushFont(c_menu::get().g_pMenuFont);
				ImGui::SetCursorPosX(9);
				ImGui::Text(crypt_str("Color "));
				ImGui::PopFont();

				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##fogcolor"), &g_cfg.esp.fog_color, NOALPHA);
			}
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 390));
		ImGui::MenuChild("World", ImVec2(316, 150));
		{
			ImGui::Checkbox(crypt_str("World modulation"), &g_cfg.esp.world_modulation);
			if (g_cfg.esp.world_modulation)
			{
				ImGui::SliderFloat(crypt_str("Bloom"), &g_cfg.esp.bloom, 0.0f, 750.0f);
				ImGui::SliderFloat(crypt_str("Exposure"), &g_cfg.esp.exposure, 0.0f, 2000.0f);
				ImGui::SliderFloat(crypt_str("Ambient"), &g_cfg.esp.ambient, 0.0f, 1500.0f);
			}
		}
		ImGui::EndChild();
	}

	if (Visuals == 3)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Info", ImVec2(334, 570));
		{
			ImGui::SetCursorPos(ImVec2(12, 10));
			if (ImGui::Button(crypt_str("Discord"), ImVec2(310, 30)))
				ShellExecute(0, 0, "https://discord.gg/9EyzbcJNqV", 0, 0, SW_SHOW);
		}
		ImGui::EndChild();
	}
}

void c_menu::misc()
{
	if (Misc == 0)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Information", ImVec2(334, 240));
		{
			ImGui::Checkbox(crypt_str("Watermark"), &g_cfg.menu.watermark);
			ImGui::Checkbox(crypt_str("Keybinds list"), &g_cfg.menu.keybinds);
			ImGui::Checkbox(crypt_str("Lightingon shot"), &g_cfg.misc.lightingonshot);
			ImGui::Checkbox(crypt_str("Holo panel"), &g_cfg.misc.holo_panel);
			ImGui::Checkbox(crypt_str("Spectators list"), &g_cfg.misc.spectators_list);
			draw_combo(crypt_str("Hitsound"), g_cfg.esp.hitsound, sounds, ARRAYSIZE(sounds));
			ImGui::Spacing();

			draw_multicombo(crypt_str("Logs"), g_cfg.misc.events_to_log, events, ARRAYSIZE(events), preview);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 340));
		ImGui::MenuChild("Spammer", ImVec2(316, 70));
		{
			ImGui::Checkbox(crypt_str("Clantag spammer"), &g_cfg.misc.clantag_spammer);
			ImGui::Checkbox(crypt_str("Chat spammer"), &g_cfg.misc.chat_spammer);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("Help", ImVec2(316, 170));
		{
			ImGui::Checkbox(crypt_str("Automatic jump"), &g_cfg.misc.bunnyhop);
			draw_combo(crypt_str("Automatic strafes"), g_cfg.misc.airstrafe, strafes, ARRAYSIZE(strafes));

			ImGui::Spacing();
			ImGui::Checkbox(crypt_str("Crouch in air"), &g_cfg.misc.crouch_in_air);
			ImGui::Checkbox(crypt_str("Fast stop"), &g_cfg.misc.fast_stop);

			draw_keybind(crypt_str("Edge jump"), &g_cfg.misc.edge_jump, crypt_str("##EDGEJUMP__HOTKEY"));
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(25, 410));
		ImGui::MenuChild("Thirdperson", ImVec2(334, 140));
		{
			draw_keybind(crypt_str("Thirdperson"), &g_cfg.misc.thirdperson_toggle, crypt_str("##TPKEY__HOTKEY"));

			if (g_cfg.misc.thirdperson_toggle.key > KEY_NONE && g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
			{
				ImGui::SliderInt(crypt_str("Thirdperson distance"), &g_cfg.misc.thirdperson_distance, 40, 300);
				ImGui::SliderInt(crypt_str("Field of view"), &g_cfg.esp.fov, 0, 100);
			}

			ImGui::Checkbox(crypt_str("Thirdperson when spectating"), &g_cfg.misc.thirdperson_when_spectating);
		}
		ImGui::EndChild();
	}

	if (Misc == 1)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Main", ImVec2(334, 190));
		{
			ImGui::Checkbox(crypt_str("Enable buybot"), &g_cfg.misc.buybot_enable);
			if (g_cfg.misc.buybot_enable)
			{
				draw_combo(crypt_str("Pistols"), g_cfg.misc.buybot2, secwep, ARRAYSIZE(secwep));
				draw_combo(crypt_str("Snipers"), g_cfg.misc.buybot1, mainwep, ARRAYSIZE(mainwep));
				draw_multicombo(crypt_str("Other"), g_cfg.misc.buybot3, grenades, ARRAYSIZE(grenades), preview);
			}
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(25, 360));
		ImGui::MenuChild("Fake-lag", ImVec2(334, 190));
		{
			ImGui::Checkbox(crypt_str("Enable"), &g_cfg.antiaim.fakelag);
			if (g_cfg.antiaim.fakelag)
			{
				draw_combo(crypt_str("Fake-lag type"), g_cfg.antiaim.fakelag_type, fakelags, ARRAYSIZE(fakelags));
				ImGui::SliderInt(crypt_str("Limit"), &g_cfg.antiaim.fakelag_amount, 1, 14);

				draw_multicombo(crypt_str("Fake-lag triggers"), g_cfg.antiaim.fakelag_enablers, lagstrigger, ARRAYSIZE(lagstrigger), preview);

				auto enabled_fakelag_triggers = false;

				for (auto i = 0; i < ARRAYSIZE(lagstrigger); i++)
					if (g_cfg.antiaim.fakelag_enablers[i])
						enabled_fakelag_triggers = true;

				if (enabled_fakelag_triggers)
					ImGui::SliderInt(crypt_str("Triggers limit"), &g_cfg.antiaim.triggers_fakelag_amount, 1, 14);
			}
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("Bypass", ImVec2(316, 50));
		{
			ImGui::Checkbox(crypt_str("Obs bypass"), &g_cfg.menu.obs_bypass);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 360));
		ImGui::MenuChild("Other", ImVec2(316, 190));
		{
			draw_keybind(crypt_str("Auto peek"), &g_cfg.misc.automatic_peek, crypt_str("##AUTOPEEK__HOTKEY"));
			ImGui::SameLine();
			ImGui::ColorEdit(crypt_str("##automatic_peek_color"), &g_cfg.misc.automatic_peek_color, ALPHA);
			ImGui::SliderInt(crypt_str("Auto peek speed"), &g_cfg.misc.automatic_peek_speed, 1, 10);
			ImGui::Checkbox(crypt_str("Fake-Duck"), &g_cfg.misc.noduck);
			if (g_cfg.misc.noduck)
				draw_keybind(crypt_str("Fake-duck"), &g_cfg.misc.fakeduck_key, crypt_str("##FAKEDUCK__HOTKEY"));
			draw_keybind(crypt_str("Slow-walk"), &g_cfg.misc.slowwalk_key, crypt_str("##SLOWWALK__HOTKEY"));
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 220));
		ImGui::MenuChild("Screen", ImVec2(316, 100));
		{
			ImGui::Checkbox(crypt_str("Unlock inventory access"), &g_cfg.misc.inventory_access);
			ImGui::Checkbox(crypt_str("Preserve killfeed"), &g_cfg.esp.preserve_killfeed);
			ImGui::Spacing();
			ImGui::SetCursorPosX(2);
			ImGui::SliderFloat(crypt_str("Aspect amount"), &g_cfg.misc.aspect_ratio_amount, 1.0f, 2.0f);
		}
		ImGui::EndChild();
	}
}

void c_menu::main()
{
	if (Main == 0)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Main", ImVec2(334, 570));
		{
			cfg_manager->config_files();
			files = cfg_manager->files;

			for (auto& current : files)
				if (current.size() > 2)
					current.erase(current.size() - 3, 3);

			ImGui::SetCursorPos(ImVec2(40, 15));
			ImGui::BeginChild("##configs", ImVec2(300, 570), false, ImGuiWindowFlags_NoBackground);
			{
				ImGui::PushFont(c_menu::get().g_pMenuFont);
				draw_config_button("Add new config", "add", false, false, false, true);
				for (int i = 0; i < files.size(); i++)
				{
					g_cfg.selected_config = i;
					bool load, save = false;
					draw_config_button(files.at(i).c_str(), std::string(files.at(i) + "id_ajdajdadmadasd").c_str(), load, save, i);
				}
				ImGui::PopFont();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("General", ImVec2(316, 570));
		{
			int child;
			auto player = players_section;
			static std::vector <Player_list_data> players;

			if (!g_cfg.player_list.refreshing)
			{
				players.clear();

				for (auto player : g_cfg.player_list.players)
					players.emplace_back(player);
			}

			static auto current_player = 0;
			if (!child)
			{
				ImGui::BeginChild(crypt_str("##ESP2_FIRST"), ImVec2(270 * dpi_scale, (child_height - 35) * dpi_scale));
				{
					if (!players.empty())
					{
						std::vector <std::string> player_names;

						for (auto player : players)
							player_names.emplace_back(player.name);

						ImGui::PushItemWidth(291 * dpi_scale);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
						ImGui::ListBoxConfigArray(crypt_str("##PLAYERLIST"), &current_player, player_names, 14);
						ImGui::PopStyleVar();
						ImGui::PopItemWidth();
					}
				}
				ImGui::EndChild();
			}
			else
			{
				ImGui::BeginChild(crypt_str("##ESP1_SECOND"), ImVec2(270 * dpi_scale, (child_height - 35) * dpi_scale));
				{
					if (!players.empty())
					{
						if (current_player >= players.size())
							current_player = players.size() - 1;

						ImGui::Checkbox(crypt_str("White list"), &g_cfg.player_list.white_list[players.at(current_player).i]);
						if (!g_cfg.player_list.white_list[players.at(current_player).i])
						{
							ImGui::Checkbox(crypt_str("High priority"), &g_cfg.player_list.high_priority[players.at(current_player).i]);
							ImGui::Checkbox(crypt_str("Force safe points"), &g_cfg.player_list.force_safe_points[players.at(current_player).i]);
							ImGui::Checkbox(crypt_str("Force body aim"), &g_cfg.player_list.force_body_aim[players.at(current_player).i]);
							ImGui::Checkbox(crypt_str("Low delta"), &g_cfg.player_list.low_delta[players.at(current_player).i]);
						}
					}

				}
				ImGui::EndChild();
			}
		}
		ImGui::EndChild();
	}

	if (Main == 1)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Main", ImVec2(334, 570));
		{
			if (should_update)
			{
				should_update = false;
				scripts = c_lua::get().scripts;

				for (auto& current : scripts)
				{
					if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
						current.erase(current.size() - 5, 5);
					else if (current.size() >= 4)
						current.erase(current.size() - 4, 4);
				}
			}

			ImGui::SetCursorPos(ImVec2(40, 15));
			ImGui::BeginChild("##Lua", ImVec2(300, 570), false, ImGuiWindowFlags_NoBackground);
			{
				ImGui::PushFont(c_menu::get().g_pMenuFont);
				draw_lua_button("Scripts", "#ddsdasdasdadsadasdawadsd", false, false, false, true);
				for (int i = 0; i < scripts.size(); i++)
				{
					selected_script = i;
					bool load, save = false;
					draw_lua_button(scripts.at(i).c_str(), std::string(scripts.at(i) + "id_ajdajdadmadasd").c_str(), load, save, i);
				}
				ImGui::PopFont();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(375, 130));
		ImGui::MenuChild("General", ImVec2(316, 570));
		{
			ImGui::BeginChild(crypt_str("##LUA_SECOND"), ImVec2(270 * dpi_scale, (child_height - 35) * dpi_scale));
			{
				auto previous_check_box = false;
				for (auto& current : c_lua::get().scripts)
				{
					auto& items = c_lua::get().items.at(c_lua::get().get_script_id(current));

					for (auto& item : items)
					{
						std::string item_name;

						auto first_point = false;
						auto item_str = false;

						for (auto& c : item.first)
						{
							if (c == '.')
							{
								if (first_point)
								{
									item_str = true;
									continue;
								}
								else
									first_point = true;
							}

							if (item_str)
								item_name.push_back(c);
						}

						switch (item.second.type)
						{
						case NEXT_LINE:
							previous_check_box = false;
							break;
						case CHECK_BOX:
							previous_check_box = true;
							ImGui::Checkbox(item_name.c_str(), &item.second.check_box_value);
							break;
						case COMBO_BOX:
							previous_check_box = false;
							draw_combo(item_name.c_str(), item.second.combo_box_value, [](void* data, int idx, const char** out_text)
								{
									auto labels = (std::vector <std::string>*)data;
									*out_text = labels->at(idx).c_str(); //-V106
									return true;
								}, &item.second.combo_box_labels, item.second.combo_box_labels.size());
							break;
						case SLIDER_INT:
							previous_check_box = false;
							ImGui::SliderInt(item_name.c_str(), &item.second.slider_int_value, item.second.slider_int_min, item.second.slider_int_max);
							break;
						case SLIDER_FLOAT:
							previous_check_box = false;
							ImGui::SliderFloat(item_name.c_str(), &item.second.slider_float_value, item.second.slider_float_min, item.second.slider_float_max);
							break;
						case COLOR_PICKER:
							if (previous_check_box)
								previous_check_box = false;
							else
								ImGui::Text((item_name + ' ').c_str());

							ImGui::SameLine();
							ImGui::ColorEdit((crypt_str("##") + item_name).c_str(), &item.second.color_picker_value, ALPHA, true);
							break;
						}
					}
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();

	}

	if (Main == 2)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Developer mode", ImVec2(334, 570));
		{
			ImGui::Checkbox(crypt_str("Developer mode"), &g_cfg.scripts.developer_mode);
			ImGui::Checkbox(crypt_str("Allow HTTP requests"), &g_cfg.scripts.allow_http);
			ImGui::Checkbox(crypt_str("Allow files read or write"), &g_cfg.scripts.allow_file);
		}
		ImGui::EndChild();
	}
}

void c_menu::skins()
{
	if (Skins == 0)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Skins", ImVec2(668, 570));
		{
			// we need to count our items in 1 line
			auto same_line_counter = 0;
			static bool drugs = false;
			static int next_id = -1;
			ImGui::SetCursorPosX(2);
			// if we didnt choose any weapon
			if (current_profile == -1)
			{
				for (auto i = 0; i < g_cfg.skins.skinChanger.size(); i++)
				{
					// do we need update our preview for some reasons?
					if (!all_skins[i])
					{
						g_cfg.skins.skinChanger.at(i).update();
						all_skins[i] = get_skin_preview(get_wep(i, (i == 0 || i == 1) ? g_cfg.skins.skinChanger.at(i).definition_override_vector_index : -1, i == 0).c_str(), g_cfg.skins.skinChanger.at(i).skin_name, device); //-V810
					}
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(51 / 255.f, 51 / 255.f, 51 / 255.f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
					ImGui::BeginGroup();
					// we licked on weapon
					if (ImGui::ImageButton(all_skins[i], ImVec2(100, 76)))
					{
						current_profile = i;
					}
					ImGui::EndGroup();
					ImGui::PopStyleColor(3);

					if (same_line_counter < 5) {
						ImGui::SameLine();
						same_line_counter++;
					}
					else {
						padding(2, 2);
						same_line_counter = 0;
					}
				}
			}
			else
			{
				// update skin preview bool
				static bool need_update[36];

				// we pressed crypt_str("Save & Close") button
				static bool leave;
				// update if we have nullptr texture or if we push force update
				if (!all_skins[current_profile] || need_update[current_profile])
				{
					all_skins[current_profile] = get_skin_preview(get_wep(current_profile, (current_profile == 0 || current_profile == 1) ? g_cfg.skins.skinChanger.at(current_profile).definition_override_vector_index : -1, current_profile == 0).c_str(), g_cfg.skins.skinChanger.at(current_profile).skin_name, device); //-V810
					need_update[current_profile] = false;
				}

				// get settings for selected weapon
				auto& selected_entry = g_cfg.skins.skinChanger[current_profile];
				selected_entry.itemIdIndex = current_profile;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(51 / 255.f, 51 / 255.f, 51 / 255.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
				ImGui::BeginGroup();
				ImGui::PushItemWidth(290);
				// search input later
				static char search_skins[64] = "\0";
				static auto item_index = selected_entry.paint_kit_vector_index;

				if (current_profile == 0)
				{
					ImGui::Text(crypt_str("Knife"));
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
					if (ImGui::Combo(crypt_str("##Knife_combo"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = game_data::knife_names[idx].name;
							return true;
						}, nullptr, IM_ARRAYSIZE(game_data::knife_names)))
						need_update[current_profile] = true; // push force update
				}
				else if (current_profile == 1)
				{
					ImGui::Text(crypt_str("Gloves"));
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
					if (ImGui::Combo(crypt_str("##Glove_combo"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = game_data::glove_names[idx].name;
							return true;
						}, nullptr, IM_ARRAYSIZE(game_data::glove_names)))
					{
						item_index = 0; // set new generated paintkits element to 0;
						need_update[current_profile] = true; // push force update
					}
				}
				else
					selected_entry.definition_override_vector_index = 0;

				if (current_profile != 1)
				{
					ImGui::Text(crypt_str("Search"));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

					if (ImGui::InputText(crypt_str("##search"), search_skins, sizeof(search_skins)))
						item_index = -1;

					ImGui::PopStyleVar();
				}

				auto main_kits = current_profile == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits;
				auto display_index = 0;

				SkinChanger::displayKits = main_kits;

				// we dont need custom gloves
				if (current_profile == 1)
				{
					for (auto i = 0; i < main_kits.size(); i++)
					{
						auto main_name = main_kits.at(i).name;

						for (auto i = 0; i < main_name.size(); i++)
							if (iswalpha((main_name.at(i))))
								main_name.at(i) = towlower(main_name.at(i));

						char search_name[64];

						if (!strcmp(game_data::glove_names[selected_entry.definition_override_vector_index].name, crypt_str("Hydra")))
							strcpy_s(search_name, sizeof(search_name), crypt_str("Bloodhound"));
						else
							strcpy_s(search_name, sizeof(search_name), game_data::glove_names[selected_entry.definition_override_vector_index].name);

						for (auto i = 0; i < sizeof(search_name); i++)
							if (iswalpha(search_name[i]))
								search_name[i] = towlower(search_name[i]);

						if (main_name.find(search_name) != std::string::npos)
						{
							SkinChanger::displayKits.at(display_index) = main_kits.at(i);
							display_index++;
						}
					}

					SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
				}
				else
				{
					if (strcmp(search_skins, crypt_str(""))) //-V526
					{
						for (auto i = 0; i < main_kits.size(); i++)
						{
							auto main_name = main_kits.at(i).name;

							for (auto i = 0; i < main_name.size(); i++)
								if (iswalpha(main_name.at(i)))
									main_name.at(i) = towlower(main_name.at(i));

							char search_name[64];
							strcpy_s(search_name, sizeof(search_name), search_skins);

							for (auto i = 0; i < sizeof(search_name); i++)
								if (iswalpha(search_name[i]))
									search_name[i] = towlower(search_name[i]);

							if (main_name.find(search_name) != std::string::npos)
							{
								SkinChanger::displayKits.at(display_index) = main_kits.at(i);
								display_index++;
							}
						}

						SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
					}
					else
						item_index = selected_entry.paint_kit_vector_index;
				}

				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
				if (!SkinChanger::displayKits.empty())
				{
					if (ImGui::ListBox(crypt_str("##PAINTKITS"), &item_index, [](void* data, int idx, const char** out_text) //-V107
						{
							while (SkinChanger::displayKits.at(idx).name.find(crypt_str("С‘")) != std::string::npos) //-V807
								SkinChanger::displayKits.at(idx).name.replace(SkinChanger::displayKits.at(idx).name.find(crypt_str("С‘")), 2, crypt_str("Рµ"));

							*out_text = SkinChanger::displayKits.at(idx).name.c_str();
							return true;
						}, nullptr, SkinChanger::displayKits.size(), SkinChanger::displayKits.size() > 9 ? 9 : SkinChanger::displayKits.size()) || !all_skins[current_profile])
					{
						SkinChanger::scheduleHudUpdate();
						need_update[current_profile] = true;

						auto i = 0;

						while (i < main_kits.size())
						{
							if (main_kits.at(i).id == SkinChanger::displayKits.at(item_index).id)
							{
								selected_entry.paint_kit_vector_index = i;
								break;
							}

							i++;
						}

					}
				}
				ImGui::PopStyleVar();
				ImGui::PushItemWidth(290 - ImGui::CalcTextSize("Seed").x - ImGui::GetStyle().ItemInnerSpacing.x);
				if (ImGui::InputInt(crypt_str("Seed"), &selected_entry.seed, 1, 100))
					SkinChanger::scheduleHudUpdate();
				ImGui::PopItemWidth();

				ImGui::PushItemWidth(290 - ImGui::CalcTextSize("StatTrak").x - ImGui::GetStyle().ItemInnerSpacing.x);
				if (ImGui::InputInt(crypt_str("StatTrak"), &selected_entry.stat_trak, 1, 15))
					SkinChanger::scheduleHudUpdate();
				ImGui::PopItemWidth();
				if (current_profile != 1)
				{
					if (!g_cfg.skins.custom_name_tag[current_profile].empty())
						strcpy_s(selected_entry.custom_name, sizeof(selected_entry.custom_name), g_cfg.skins.custom_name_tag[current_profile].c_str());
					ImGui::SetCursorPosX(8);
					ImGui::Text(crypt_str("Name Tag"));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					ImGui::SetCursorPosX(8);
					if (ImGui::InputText(crypt_str("##nametag"), selected_entry.custom_name, sizeof(selected_entry.custom_name)))
					{
						g_cfg.skins.custom_name_tag[current_profile] = selected_entry.custom_name;
						SkinChanger::scheduleHudUpdate();
					}

					ImGui::PopStyleVar();
				}

				ImGui::PopItemWidth();
				ImGui::EndGroup();

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 305, 5));

				ImGui::BeginChild("##seett", ImVec2(280, 400));
				ImGui::PushItemWidth(240);
				if (ImGui::SliderFloat(crypt_str("Wear"), &selected_entry.wear, 0.0f, 1.0f))
					drugs = true;
				else if (drugs)
				{
					SkinChanger::scheduleHudUpdate();
					drugs = false;
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6 * c_menu::get().dpi_scale);
				ImGui::SetCursorPosX(8);
				ImGui::Text(crypt_str("Quality"));
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
				if (ImGui::Combo(crypt_str("##Quality_combo"), &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = game_data::quality_names[idx].name;
						return true;
					}, nullptr, IM_ARRAYSIZE(game_data::quality_names)))
					SkinChanger::scheduleHudUpdate();

					ImGui::PopItemWidth();
					ImGui::Checkbox(crypt_str("Force rare animations"), &g_cfg.skins.rare_animations);
					ImGui::SetCursorPosX(8);
					if (ImGui::ImageButton(all_skins[current_profile], ImVec2(256, 214)))
					{
						// maybe i will do smth later where, who knows :/
					}
					ImGui::SetCursorPosX(8);
					if (ImGui::CustomButton(crypt_str("Close"), crypt_str("##CLOSE__SKING"), ImVec2(264, 20)))
					{
						next_id = -1;
						leave = true;
					}
					//

					ImGui::EndChild();
					ImGui::PopStyleColor(3);
					// update element
					selected_entry.update();

					// we need to reset profile in the end to prevent render images with massive's index == -1
					if (leave)
					{
						ImGui::SetScrollY(0);
						current_profile = next_id;
						leave = false;
					}
			}
		}
		ImGui::EndChild();
	}

	if (Skins == 1)
	{
		ImGui::SetCursorPos(ImVec2(25, 130));
		ImGui::MenuChild("Agents", ImVec2(668, 570));
		{
			draw_combo(crypt_str("Player models"), g_cfg.player.player_models, player_models, ARRAYSIZE(player_models));
		}
		ImGui::EndChild();
	}
}

void c_menu::sub_tabs()
{
	if (tab == 0)
	{
		ImGui::SetCursorPos(ImVec2{ 407 , 45 });
		if (ImGui::tab(" General", "", !Rage_Tab)) Rage_Tab = 0;

		ImGui::SetCursorPos(ImVec2{ 554 , 45 });
		if (ImGui::tab("   Other", " ", Rage_Tab == 1)) Rage_Tab = 1;
	}

	if (tab == 1)
	{
		ImGui::SetCursorPos(ImVec2{ 407 , 45 });
		if (ImGui::tab(" General", "", !Legit_Tab)) Legit_Tab = 0;
	}

	if (tab == 2)
	{
		ImGui::SetCursorPos(ImVec2{ 57 , 45 });
		if (ImGui::tab(" General", "", !Visuals)) Visuals = 0;

		ImGui::SetCursorPos(ImVec2{ 204 , 45 });
		if (ImGui::tab("    Main", " ", Visuals == 1)) Visuals = 1;

		ImGui::SetCursorPos(ImVec2{ 407 , 45 });
		if (ImGui::tab("   Other", "  ", Visuals == 2)) Visuals = 2;

		ImGui::SetCursorPos(ImVec2{554 , 45});
		if (ImGui::tab("    Info", "   ", Visuals == 3)) Visuals = 3;
	}

	if (tab == 3)
	{
		ImGui::SetCursorPos(ImVec2{ 407 , 45 });
		if (ImGui::tab(" General", "", !Misc)) Misc = 0;

		ImGui::SetCursorPos(ImVec2{ 554 , 45 });
		if (ImGui::tab("   Other", " ", Misc == 1)) Misc = 1;
	}

	if (tab == 4)
	{
		ImGui::SetCursorPos(ImVec2{ 407 , 45 });
		if (ImGui::tab("  Config ", "", !Main)) Main = 0;

		ImGui::SetCursorPos(ImVec2{ 554 , 45 });
		if (ImGui::tab("      Lua", " ", Main == 1)) Main = 1;

		ImGui::SetCursorPos(ImVec2{ 204 , 45 });
		if (ImGui::tab("  Debug", "  ", Main == 2)) Main = 2;
	}

	if (tab == 5)
	{
		ImGui::SetCursorPos(ImVec2{ 407 , 45 });
		if (ImGui::tab("    Skins", "", !Skins)) Skins = 0;

		ImGui::SetCursorPos(ImVec2{ 554 , 45 });
		if (ImGui::tab("  Agents", " ", Skins == 1)) Skins = 1;
	}
}

void c_menu::tabs()
{
	// [700, 750]
	ImGui::SetCursorPos(ImVec2{ 10 , 725 });
	if (ImGui::tab("      RAGE", "A", !tab)) tab = 0;

	ImGui::SetCursorPos(ImVec2{ 120 , 725 });
	if (ImGui::tab("     LEGIT", "B", tab == 1)) tab = 1;

	// 110 + 10
	ImGui::SetCursorPos(ImVec2{ 230 , 725 });
	if (ImGui::tab("    VISUAL", "K", tab == 2)) tab = 2;

	// 110 + 110 + 10
	ImGui::SetCursorPos(ImVec2{ 340 , 725 });
	if (ImGui::tab("      MISC", "I", tab == 3)) tab = 3;

	// 110 + 110 + 110 + 10
	ImGui::SetCursorPos(ImVec2{ 450 , 725 });
	if (ImGui::tab("      MAIN", "O", tab == 4)) tab = 4;

	ImGui::SetCursorPos(ImVec2{ 560 , 725 });
	if (ImGui::tab("	    SKINS", "C", tab == 5)) tab = 5;

	switch (tab)
	{
		case 0: rage(); break;
		case 1: legit(); break;
		case 2: visuals(); break;
		case 3: misc(); break;
		case 4: main(); break;
		case 5: skins(); break;
	}
}

void c_menu::draw(bool is_open)
{
	static auto w = 0, h = 0, current_h = 0;
	m_engine()->GetScreenSize(w, current_h);

	if (h != current_h)
	{
		if (h)
			update_scripts = true;

		h = current_h;
		update_dpi = true;
	}

	int m_alpha = is_open ? 1.f : 0.f;

	public_alpha = m_alpha;

	if (m_alpha <= 0.00005f)
		return;

	if (!menu_setupped)
		menu_setup(ImGui::GetStyle());

	auto& ss = ImGui::GetStyle();
	ss.FrameRounding = 6;
	ss.ChildRounding = 10;
	ss.PopupRounding = 5;
	ss.ScrollbarRounding = 0;
	ss.ScrollbarSize = 5;
	ss.FramePadding = ImVec2(2, 2);

	auto c = ss.Colors;
	c[ImGuiCol_FrameBg] = ImColor(25, 25, 25);
	c[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 25);
	c[ImGuiCol_FrameBgActive] = ImColor(100, 125, 200);
	c[ImGuiCol_Separator] = ImColor(100, 125, 200);
	c[ImGuiCol_SliderGrab] = ImColor(100, 125, 200);
	c[ImGuiCol_SliderGrabActive] = ImColor(100, 125, 200);
	c[ImGuiCol_PopupBg] = ImColor(25, 25, 25);
	c[ImGuiCol_Header] = ImColor(25, 25, 25);
	c[ImGuiCol_HeaderHovered] = ImColor(100, 125, 200);
	c[ImGuiCol_HeaderActive] = ImColor(100, 125, 200);
	c[ImGuiCol_Button] = ImColor(25, 25, 25);
	c[ImGuiCol_ButtonHovered] = ImColor(100, 125, 200);
	c[ImGuiCol_ButtonActive] = ImColor(100, 125, 200);
	c[ImGuiCol_Text] = ImColor(255, 255, 255);
	c[ImGuiCol_TextDisabled] = ImColor(100, 125, 200);

	auto& time = g_ctx.globals.time;

	{
//		ImGui::ShowDemoWindow();
		ImGui::SetNextWindowSize(ImVec2(gs));
		if (ImGui::Begin(("##"), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
		{
			s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
			p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);
			auto draw = ImGui::GetWindowDrawList();

			float dpi_scale = 1.f;

			//беграунт
			draw->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 700, p.y + 750), ImColor(25, 25, 25), 6);
			{
				//обводка
				draw->AddLine(ImVec2(p.x, p.y + 710), ImVec2(p.x + 750, p.y + 710), ImColor(255, 255, 255));
				draw->AddRect(ImVec2(p.x - 1, p.y), ImVec2(p.x + 701, p.y + 751), ImColor(33, 33, 33), 6);
				draw->AddRect(ImVec2(p.x - 2, p.y), ImVec2(p.x + 702, p.y + 752), ImColor(40, 40, 40), 6);

				//суб таб
				draw->AddRectFilled(ImVec2(p.x + 20, p.y + 26), ImVec2(p.x + 350, p.y + 75), ImColor(20, 20, 20), 6);
				{
					draw->AddRect(ImVec2(p.x + 19, p.y + 26), ImVec2(p.x + 351, p.y + 76), ImColor(33, 33, 33), 6);
					draw->AddRect(ImVec2(p.x + 20, p.y + 27), ImVec2(p.x + 352, p.y + 77), ImColor(40, 40, 40), 6);
				}
				//оружия
				draw->AddRectFilled(ImVec2(p.x + 370, p.y + 26), ImVec2(p.x + 680, p.y + 75), ImColor(20, 20, 20), 6);
				{
					draw->AddRect(ImVec2(p.x + 369, p.y + 26), ImVec2(p.x + 681, p.y + 76), ImColor(33, 33, 33), 6);
					draw->AddRect(ImVec2(p.x + 370, p.y + 27), ImVec2(p.x + 682, p.y + 77), ImColor(40, 40, 40), 6);
				}

				//функции
				{
					tabs();
					sub_tabs();
				}
			}
		}
		ImGui::End();

	}
}
