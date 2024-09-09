using UnrealBuildTool;

public class BloxFileManager : ModuleRules
{
	public BloxFileManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] {  });
		PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "Bloxorz" });
	}
}
