#pragma once

struct RegistryManager;

struct AppContext {
	AppContext(RegistryManager& rm) : RegistryManager(rm) {}

	RegistryManager& RegistryManager;
};
