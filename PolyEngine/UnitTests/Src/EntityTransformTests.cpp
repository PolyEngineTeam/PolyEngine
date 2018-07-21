#include <Defines.hpp>
#include <catch.hpp>

#include <ECS/EntityTransform.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;


//									 _____e0________________
//									/			\			\
//						    	___e1__			 e2		     e3
//						       /	   \					   \	
//						     e4		    e5						e7
//										  \						  \
//											e6			   ________e8_____________
//														  /          \            \
//														e9           e10          e11	
//


TEST_CASE("Checking equality of transforms after applying global and local translation rotation and scale.", "[GlobalLocalTransform]")
{
	Entity* e[12];

	e[0] = new Entity();	// root
	(e[1] = new Entity())->SetParent(e[0]);
	(e[4] = new Entity())->SetParent(e[1]);
	(e[5] = new Entity())->SetParent(e[1]);
	(e[6] = new Entity())->SetParent(e[5]);
	(e[2] = new Entity())->SetParent(e[0]);
	(e[3] = new Entity())->SetParent(e[0]);
	(e[7] = new Entity())->SetParent(e[3]);
	(e[8] = new Entity())->SetParent(e[7]);
	(e[9] = new Entity())->SetParent(e[8]);
	(e[10] = new Entity())->SetParent(e[8]);
	(e[11] = new Entity())->SetParent(e[8]);

	SECTION("Local and global set translation.")
	{
		for (int a = 0; a < 12; ++a)
			for (int b = 0; b < 12; ++b)
			{
				REQUIRE(e[a]->GetTransform().GetGlobalTranslation() == e[b]->GetTransform().GetGlobalTranslation());
				REQUIRE(e[a]->GetTransform().GetGlobalRotation() == e[b]->GetTransform().GetGlobalRotation());
				REQUIRE(e[a]->GetTransform().GetGlobalScale() == e[b]->GetTransform().GetGlobalScale());

				REQUIRE(e[a]->GetTransform().GetLocalTranslation() == e[b]->GetTransform().GetLocalTranslation());
				REQUIRE(e[a]->GetTransform().GetLocalRotation() == e[b]->GetTransform().GetLocalRotation());
				REQUIRE(e[a]->GetTransform().GetLocalScale() == e[b]->GetTransform().GetLocalScale());
			}
	}
}