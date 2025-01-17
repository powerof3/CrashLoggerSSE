#include "Crash/Introspection/Introspection.h"

#include "Crash/Modules/ModuleHandler.h"
#include "Crash/PDB/PdbHandler.h"
#define MAGIC_ENUM_RANGE_MAX 256

namespace Crash::Introspection::SSE
{
	using filter_results = std::vector<std::pair<std::string, std::string>>;

	[[nodiscard]] std::string quoted(std::string_view a_str)
	{
		return fmt::format("\"{}\""sv, a_str);
	}

	template <class T = RE::TESForm>
	class TESForm
	{
	public:
		using value_type = T;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto form = static_cast<const value_type*>(a_ptr);

			try {
				const auto file = form->GetDescriptionOwnerFile();
				const auto filename = file ? file->GetFilename() : ""sv;
				if (!filename.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}File"sv,
							"",
							tab_depth),
						quoted(filename));
			} catch (...) {}

			try {
				const auto formFlags = form->GetFormFlags();
				using recordFlags = value_type::RecordFlags::RecordFlag;
				std::string flagString = "";
				constexpr auto flagEntries = magic_enum::enum_entries<recordFlags>();
				for (const auto& entry : flagEntries) {
					const auto flag = entry.first;
					const auto flagName = entry.second;
					if (flag & formFlags)
						flagString = flagString.empty() ?
						flagName:
						flagString.append(" | "sv).append(flagName);
				}
				a_results.emplace_back(
					fmt::format(
						"{:\t>{}}Flags"sv,
						"",
						tab_depth),
					fmt::format(
						"0x{:08X} {}"sv,
						formFlags,
						flagString));
			} catch (...) {}

			try {
				const auto name = form->GetName();
				if (name && name[0])
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Name"sv,
							"",
							tab_depth),
						quoted(name));
			} catch (...) {}

			try {
				const auto editorID = form->GetFormEditorID();
				if (editorID && editorID[0])
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}EditorID"sv,
							"",
							tab_depth),
						quoted(editorID));
			} catch (...) {}

			try {
				const auto formID = form->GetFormID();
				a_results.emplace_back(
					fmt::format(
						"{:\t>{}}FormID"sv,
						"",
						tab_depth),
					fmt::format(
						"0x{:08X}"sv,
						formID));
			} catch (...) {}

			try {
				const auto formType = form->GetFormType();
				const auto formTypeName = magic_enum::enum_name(formType);
				if (!formTypeName.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}FormType"sv,
							"",
							tab_depth),
						formTypeName);
			} catch (...) {}
		}
	};

	class TESFullName
	{
	public:
		using value_type = RE::TESFullName;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto object = static_cast<const value_type*>(a_ptr);

			if (!object)
				return;
			try {
				const auto name = object->GetFullName();
				if (name && name[0])
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}GetFullName"sv,
							"",
							tab_depth),
						quoted(name));
			} catch (...) {}
		}
	};

	class BSShaderProperty
	{
	public:
		using value_type = RE::BSShaderProperty;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto form = static_cast<const value_type*>(a_ptr);

			try {
				const auto formFlags = form->flags;
				a_results.emplace_back(
					fmt::format(
						"{:\t>{}}Flags"sv,
						"",
						tab_depth),
					fmt::format(
						"0x{:08X}"sv,
						formFlags.get()));
			} catch (...) {}
			try {
				const auto name = form->name.c_str();
				if (name && name[0])
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Name"sv,
							"",
							tab_depth),
						quoted(name));
			} catch (...) {}
			try {
				const auto rttiname = form->GetRTTI() ? form->GetRTTI()->GetName() : ""sv;
				if (!rttiname.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}RTTIName"sv,
							"",
							tab_depth),
						quoted(rttiname));
			} catch (...) {}
			try {
				const auto formType = form->GetType();
				a_results.emplace_back(
					fmt::format(
						"{:\t>{}}NiPropertyType"sv,
						"",
						tab_depth),
					fmt::format(
						"{:02}"sv,
						formType));
			} catch (...) {}
			try {
				for (auto i = 0; i < form->GetExtraDataSize(); i++) {
					const auto extraData = form->GetExtraDataAt(i);
					if (!extraData->GetName().empty()) {
						const auto name = extraData->GetName().c_str();
						if (name && name[0])
							a_results.emplace_back(
								fmt::format(
									"{:\t>{}}ExtraData[{}] Name"sv,
									"",
									tab_depth,
									i),
								quoted(name));
					}
				}
			} catch (...) {}


		}
	};
	class NiAVObject
	{
	public:
		using value_type = RE::NiAVObject;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto object = static_cast<const value_type*>(a_ptr);
			if (!object)
				return;
			try {
				const auto name = object ? object->name.c_str() : ""sv;
				if (!name.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Name"sv,
							"",
							tab_depth),
						quoted(name));
			} catch (...) {}

			try {
				const auto name = object->GetRTTI() ? object->GetRTTI()->GetName() : ""sv;
				if (!name.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}RTTIName"sv,
							"",
							tab_depth),
						quoted(name));
			} catch (...) {}

			try {
				for (auto i = 0; i < object->GetExtraDataSize(); i++) {
					const auto extraData = object->GetExtraDataAt(i);
					if (!extraData->GetName().empty()) {
						const auto name = extraData->GetName().c_str();
						if (name && name[0])
							a_results.emplace_back(
								fmt::format(
									"{:\t>{}}ExtraData[{}] Name"sv,
									"",
									tab_depth,
									i),
								quoted(name));
					}

				}
			} catch (...) {}

			try {
				const auto userdata = object->GetUserData();
				if (userdata) {
					const auto name = userdata->GetDisplayFullName();
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Full Name"sv,
							"",
							tab_depth),
						quoted(name));
					const auto objectref = userdata->GetObjectReference();
					const auto filename = objectref && objectref->As<RE::TESModel>() ? objectref->As<RE::TESModel>()->GetModel() :
                                                                                       ""sv;
					if (!filename.empty())
						a_results.emplace_back(
							fmt::format(
								"{:\t>{}}File"sv,
								"",
								tab_depth),
							quoted(filename));
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Checking User Data"sv,
							"",
							tab_depth),
						"-----");
					TESForm<RE::TESObjectREFR>::filter(a_results, userdata, tab_depth + 1);
					if (auto owner = userdata->GetOwner()) {
						a_results.emplace_back(
							fmt::format(
								"{:\t>{}}Checking Owner"sv,
								"",
								tab_depth),
							"-----");
						TESForm<RE::TESForm>::filter(a_results, owner, tab_depth + 1);
					}
				}
			} catch (...) {}
			try {
				const auto parent = object->parent;
				const auto parentIndex = object->parentIndex;
				if (parent) {
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Checking Parent"sv,
							"",
							tab_depth),
						fmt::format(
							"{}"sv, parentIndex));
					filter(a_results, parent, tab_depth + 1);
				}
			} catch (...) {}
		}
	};

	class NiStream
	{
	public:
		using value_type = RE::NiStream;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto object = static_cast<const value_type*>(a_ptr);
			if (!object)
				return;
			try {
				const auto& header = object->header;
				a_results.emplace_back(
					fmt::format(
						"{:\t>{}}Header"sv,
						"",
						tab_depth),
					fmt::format(
						"author: {} version: {} processScript: {} exportScript: {}",
						header.author,
						header.version,
						header.processScript,
						header.exportScript));
			} catch (...) {}

			try {
				const auto lastLoadedRTTI = object->lastLoadedRTTI;
				if (lastLoadedRTTI && lastLoadedRTTI[0])
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}lastLoadedRTTI"sv,
							"",
							tab_depth),
						quoted(lastLoadedRTTI));
			} catch (...) {}

			try {
				const auto inputFilePath = object->inputFilePath;
				if (inputFilePath && inputFilePath[0])
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}inputFilePath"sv,
							"",
							tab_depth),
						quoted(inputFilePath));
			} catch (...) {}

			try {
				const auto filePath = object->filePath;
				if (filePath && filePath[0])
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}filePath"sv,
							"",
							tab_depth),
						quoted(filePath));
			} catch (...) {}
		}
	};
	class BSShaderMaterial
	{
	public:
		using value_type = RE::BSShaderMaterial;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto object = static_cast<const value_type*>(a_ptr);
			if (!object)
				return;
			try {
				const auto& feature = object->GetFeature();
				const auto featureName = magic_enum::enum_name(feature);
				if (!featureName.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Feature"sv,
							"",
							tab_depth),
						featureName);
			} catch (...) {}

			try {
				const auto type = object->GetType();
				const auto typeName = magic_enum::enum_name(type);
				if (!typeName.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Type"sv,
							"",
							tab_depth),
						quoted(typeName));
			} catch (...) {}
		}
	};

	class hkbCharacter
	{
	public:
		using value_type = RE::hkbCharacter;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto object = static_cast<const value_type*>(a_ptr);
			if (!object)
				return;
			try {
				const auto& name = object->name;
				if (!name.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Name"sv,
							"",
							tab_depth),
						quoted(name.data()));
			} catch (...) {}
		};
	};

	class hkbNode
	{
	public:
		using value_type = RE::hkbNode;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto object = static_cast<const value_type*>(a_ptr);
			if (!object)
				return;
			try {
				const auto& name = object->name;
				if (!name.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Name"sv,
							"",
							tab_depth),
						quoted(name.data()));
			} catch (...) {}
			try {
				const auto& id = object->id;
				a_results.emplace_back(
					fmt::format(
						"{:\t>{}}ID"sv,
						"",
						tab_depth),
					fmt::format(
						"0x{:08X}"sv,
						id));
			} catch (...) {}
		};
	};

class hkpWorldObject
	{
	public:
		using value_type = RE::hkpWorldObject;

		static void filter(
			filter_results& a_results,
			const void* a_ptr, int tab_depth = 0) noexcept
		{
			const auto object = static_cast<const value_type*>(a_ptr);
			if (!object)
				return;
			try {
				const auto& name = object->name;
				if (!name.empty())
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Name"sv,
							"",
							tab_depth),
						quoted(name.data()));
			} catch (...) {}
			try {
				const auto userdata = object->GetUserData();
				if (userdata) {
					const auto name = userdata->GetDisplayFullName();
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Full Name"sv,
							"",
							tab_depth),
						quoted(name));
					const auto objectref = userdata->GetObjectReference();
					const auto filename = objectref && objectref->As<RE::TESModel>() ? objectref->As<RE::TESModel>()->GetModel() :
                                                                                       ""sv;
					if (!filename.empty())
						a_results.emplace_back(
							fmt::format(
								"{:\t>{}}File"sv,
								"",
								tab_depth),
							quoted(filename));
					a_results.emplace_back(
						fmt::format(
							"{:\t>{}}Checking User Data"sv,
							"",
							tab_depth),
						"-----");
					TESForm<RE::TESObjectREFR>::filter(a_results, userdata, tab_depth + 1);
					if (auto owner = userdata->GetOwner()) {
						a_results.emplace_back(
							fmt::format(
								"{:\t>{}}Checking Owner"sv,
								"",
								tab_depth),
							"-----");
						TESForm<RE::TESForm>::filter(a_results, owner, tab_depth + 1);
					}
				}
			} catch (...) {}

		};
	};

}

namespace Crash::Introspection
{
	[[nodiscard]] const Modules::Module* get_module_for_pointer(
		const void* a_ptr,
		std::span<const module_pointer> a_modules) noexcept
	{
		const auto it = std::lower_bound(
			a_modules.rbegin(),
			a_modules.rend(),
			reinterpret_cast<std::uintptr_t>(a_ptr),
			[](auto&& a_lhs, auto&& a_rhs) noexcept {
				return a_lhs->address() >= a_rhs;
			});
		return it != a_modules.rend() && (*it)->in_range(a_ptr) ? it->get() : nullptr;
	}

	namespace detail
	{
		class Integer
		{
		public:
			Integer(std::size_t a_value) noexcept :
				_value(a_value),
				name_string(a_value >> 63 ?
								fmt::format("(size_t) [uint: {} int: {}]"s, _value, static_cast<std::make_signed_t<size_t>>(_value)) :
                                fmt::format("(size_t) [{}]"s, _value))
			{
			}

			[[nodiscard]] std::string name() const { return name_string; }

		private:
			const std::size_t _value;
			const std::string name_string;
		};

		class Pointer
		{
		public:
			Pointer() noexcept = default;

			Pointer(const void* a_ptr, std::span<const module_pointer> a_modules) noexcept :
				_module(get_module_for_pointer(a_ptr, a_modules))
			{
				if (_module) {
					_ptr = a_ptr;
				}
			}

			[[nodiscard]] std::string name() const
			{
				if (_module) {
					const auto address = reinterpret_cast<std::uintptr_t>(_ptr);
					const auto pdbDetails = Crash::PDB::pdb_details(_module->path(), address - _module->address());
					if (!pdbDetails.empty())
						return fmt::format(
							"(void* -> {}+{:07X} -> {})"sv,
							_module->name(),
							address - _module->address(),
							pdbDetails);
					return fmt::format(
						"(void* -> {}+{:07X})"sv,
						_module->name(),
						address - _module->address());
				} else {
					return "(void*)"s;
				}
			}

		private:
			const Modules::Module* _module{ nullptr };
			const void* _ptr{ nullptr };
		};

		class Polymorphic
		{
		public:
			explicit Polymorphic(std::string_view a_mangled) noexcept :
				_mangled{ a_mangled }
			{
				// NOLINTNEXTLINE(readability-simplify-subscript-expr)
				assert(_mangled.size() > 1 && _mangled.data()[_mangled.size()] == '\0');
			}

			[[nodiscard]] std::string name() const
			{
				const auto demangle = [](const char* a_in, char* a_out, std::uint32_t a_size) {
					static std::mutex m;
					std::lock_guard l{ m };
					return ::WinAPI::UnDecorateSymbolName(
						a_in,
						a_out,
						a_size,
						(::WinAPI::UNDNAME_NO_MS_KEYWORDS) |
							(::WinAPI::UNDNAME_NO_FUNCTION_RETURNS) |
							(::WinAPI::UNDNAME_NO_ALLOCATION_MODEL) |
							(::WinAPI::UNDNAME_NO_ALLOCATION_LANGUAGE) |
							(::WinAPI::UNDNAME_NO_THISTYPE) |
							(::WinAPI::UNDNAME_NO_ACCESS_SPECIFIERS) |
							(::WinAPI::UNDNAME_NO_THROW_SIGNATURES) |
							(::WinAPI::UNDNAME_NO_RETURN_UDT_MODEL) |
							(::WinAPI::UNDNAME_NAME_ONLY) |
							(::WinAPI::UNDNAME_NO_ARGUMENTS) |
							static_cast<std::uint32_t>(0x8000));  // Disable enum/class/struct/union prefix
				};

				std::array<char, 0x1000> buf{ '\0' };
				const auto len = demangle(
					_mangled.data() + 1,
					buf.data(),
					static_cast<std::uint32_t>(buf.size()));

				if (len != 0) {
					return fmt::format(
						"({}*)"sv,
						std::string_view{ buf.data(), len });
				} else {
					return "(ERROR)"s;
				}
			}

		private:
			std::string_view _mangled;
		};

		class F4Polymorphic
		{
		public:
			F4Polymorphic(
				std::string_view a_mangled,
				const RE::RTTI::CompleteObjectLocator* a_col,
				const void* a_ptr) noexcept :
				_poly{ a_mangled },
				_col{ a_col },
				_ptr{ a_ptr }
			{
				assert(_col != nullptr);
				assert(_ptr != nullptr);
			}

			[[nodiscard]] std::string name() const
			{
				auto result = _poly.name();
				SSE::filter_results xInfo;

				const auto moduleBase = REL::Module::get().base();
				const auto hierarchy = _col->classDescriptor.get();
				const std::span bases(
					reinterpret_cast<std::uint32_t*>(hierarchy->baseClassArray.offset() + moduleBase),
					hierarchy->numBaseClasses);
				for (const auto rva : bases) {
					const auto base = reinterpret_cast<RE::RTTI::BaseClassDescriptor*>(rva + moduleBase);
					const auto it = FILTERS.find(base->typeDescriptor->mangled_name());
					if (it != FILTERS.end()) {
						const auto root = util::adjust_pointer<void>(_ptr, -static_cast<std::ptrdiff_t>(_col->offset));
						const auto target = util::adjust_pointer<void>(root, static_cast<std::ptrdiff_t>(base->pmd.mDisp));
						it->second(xInfo, target, 0);
					} else
						logger::info("Found unhandled type:\t{}\t{}"sv, result, base->typeDescriptor->mangled_name());
				}

				if (!xInfo.empty()) {
					for (const auto& [key, value] : xInfo) {
						result += fmt::format(
							"\n\t\t{}: {}"sv,
							key,
							value);
					}
				}

				return result;
			}

		private:
			static constexpr auto FILTERS = frozen::make_map({
				std::make_pair(".?AVPlayerCharacter@@"sv, SSE::TESForm<RE::PlayerCharacter>::filter),
				std::make_pair(".?AVCharacter@@"sv, SSE::TESForm<RE::Character>::filter),
				std::make_pair(".?AVTESObjectCELL@@"sv, SSE::TESForm<RE::TESObjectCELL>::filter),
				std::make_pair(".?AVTESForm@@"sv, SSE::TESForm<RE::TESForm>::filter),
				std::make_pair(".?AVNiAVObject@@"sv, SSE::NiAVObject::filter),
				std::make_pair(".?AVBSShaderProperty@@"sv, SSE::BSShaderProperty::filter),
				std::make_pair(".?AVNiStream@@"sv, SSE::NiStream::filter),
				std::make_pair(".?AVTESFullName@@"sv, SSE::TESFullName::filter),
				std::make_pair(".?AVBSShaderMaterial@@"sv, SSE::BSShaderMaterial::filter),
				std::make_pair(".?AVhkbNode@@"sv, SSE::hkbNode::filter),
				std::make_pair(".?AVhkbCharacter@@"sv, SSE::hkbCharacter::filter),
				std::make_pair(".?AVhkpWorldObject@@"sv, SSE::hkpWorldObject::filter),
			});

			Polymorphic _poly;
			const RE::RTTI::CompleteObjectLocator* _col{ nullptr };
			const void* _ptr{ nullptr };
		};

		class String
		{
		public:
			String(std::string_view a_str) noexcept :
				_str(a_str)
			{}

			[[nodiscard]] std::string name() const
			{
				return fmt::format("(char*) \"{}\""sv, _str);
			}

		private:
			std::string_view _str;
		};

		using analysis_result = std::variant<
			Integer,
			Pointer,
			Polymorphic,
			F4Polymorphic,
			String>;

		template <class T, class... Args>
		[[nodiscard]] analysis_result make_result(Args&&... a_args) noexcept(
			std::is_nothrow_constructible_v<T, Args...>)
		{
			return analysis_result(std::in_place_type_t<T>{}, std::forward<Args>(a_args)...);
		}

		[[nodiscard]] auto analyze_polymorphic(
			void* a_ptr,
			std::span<const module_pointer> a_modules) noexcept
			-> std::optional<analysis_result>
		{
			try {
				const auto vtable = *reinterpret_cast<void**>(a_ptr);
				const auto mod = get_module_for_pointer(vtable, a_modules);
				if (!mod || !mod->in_rdata_range(vtable)) {
					return std::nullopt;
				}

				const auto col =
					*reinterpret_cast<RE::RTTI::CompleteObjectLocator**>(
						reinterpret_cast<std::size_t*>(vtable) - 1);
				if (mod != get_module_for_pointer(col, a_modules) || !mod->in_rdata_range(col)) {
					return std::nullopt;
				}

				const auto typeDesc =
					reinterpret_cast<RE::RTTI::TypeDescriptor*>(
						mod->address() + col->typeDescriptor.offset());
				if (mod != get_module_for_pointer(typeDesc, a_modules) || !mod->in_data_range(typeDesc)) {
					return std::nullopt;
				}

				if (*reinterpret_cast<const void**>(typeDesc) != mod->type_info()) {
					return std::nullopt;
				}

				if (_stricmp(mod->name().data(), util::module_name().c_str()) == 0) {
					return make_result<F4Polymorphic>(typeDesc->mangled_name(), col, a_ptr);
				} else {
					return make_result<Polymorphic>(typeDesc->mangled_name());
				}
			} catch (...) {
				return std::nullopt;
			}
		}

		[[nodiscard]] auto analyze_string(void* a_ptr) noexcept
			-> std::optional<analysis_result>
		{
			try {
				const auto printable = [](char a_ch) noexcept {
					if (' ' <= a_ch && a_ch <= '~') {
						return true;
					} else {
						switch (a_ch) {
						case '\t':
						case '\n':
							return true;
						default:
							return false;
						}
					}
				};

				const auto str = static_cast<const char*>(a_ptr);
				constexpr std::size_t max = 1000;
				std::size_t len = 0;
				for (; len < max && str[len] != '\0'; ++len) {
					if (!printable(str[len])) {
						return std::nullopt;
					}
				}

				if (len == 0 || len >= max) {
					return std::nullopt;
				}

				return make_result<String>(std::string_view{ str, len });
			} catch (...) {
				return std::nullopt;
			}
		}

		[[nodiscard]] auto analyze_pointer(
			void* a_ptr,
			std::span<const module_pointer> a_modules) noexcept
			-> analysis_result
		{
			if (auto poly = analyze_polymorphic(a_ptr, a_modules); poly) {
				return *std::move(poly);
			}

			if (auto str = analyze_string(a_ptr); str) {
				return *std::move(str);
			}

			return make_result<Pointer>(a_ptr, a_modules);
		}

		[[nodiscard]] auto analyze_integer(
			std::size_t a_value,
			std::span<const module_pointer> a_modules) noexcept
			-> analysis_result
		{
			try {
				if (a_value != 0) {
					*reinterpret_cast<const volatile std::byte*>(a_value);
					return analyze_pointer(reinterpret_cast<void*>(a_value), a_modules);
				}
			} catch (...) {}

			return make_result<Integer>(a_value);
		}
	}

	std::vector<std::string> analyze_data(
		std::span<const std::size_t> a_data,
		std::span<const module_pointer> a_modules)
	{
		std::vector<std::string> results;
		results.resize(a_data.size());
		std::for_each(
			std::execution::par_unseq,
			a_data.begin(),
			a_data.end(),
			[&](auto& a_val) {
				const auto result = detail::analyze_integer(a_val, a_modules);
				const auto pos = std::addressof(a_val) - a_data.data();
				results[pos] = std::visit(
					[](const auto& a_analysis) { return a_analysis.name(); },
					result);
			});
		return results;
	}
}
