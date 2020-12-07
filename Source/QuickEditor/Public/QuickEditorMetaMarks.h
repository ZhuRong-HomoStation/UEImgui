#pragma once

namespace UM
{
	enum QE_UF
	{
		// add menu item to main menu(Level editor)
		// function type: static 
		// function signature: void(void)
		// meta value: menu path eg. "/Test/SubMenu/Item" 
		QEMenuItem ,

		// add menu item to asset menu(right click asset icon) 
		// function type: member 
		QEAssetItem ,

		// add menu item to actor menu(right click Actor) 
		// function type: member 
		QEActorItem ,
	};
}

