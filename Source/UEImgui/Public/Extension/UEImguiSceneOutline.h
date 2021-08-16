#pragma once

DECLARE_DELEGATE_RetVal_OneParam(bool, FActorFilter, AActor*);

struct FUEImguiSceneNode
{
	AActor*		Actor;
	bool		bCulled;
	bool		bDisabled;
};

class FUEImguiSceneOutline
{
	FUEImguiSceneOutline(UObject* InWorldContext = GWorld);
	~FUEImguiSceneOutline();

public:
	// States 
	FActorFilter				Filter;
	TArray<FUEImguiSceneNode>	SceneNodes;
	TArray<FUEImguiSceneNode*>	Entries;
	TArray<AActor*>				SelectedActors;
	bool						bNeedToUpdate;
	
	// Bound info
	TWeakObjectPtr<UWorld>		BoundWorld;
	FDelegateHandle				LevelAddedHandle;
	FDelegateHandle				LevelRemovedHandle;
	FDelegateHandle				ActorSpawnHandle;
};

