#include <EnginePCH.hpp>
#include <Sequences/Data/Sequence.hpp>

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::Sequence)

//------------------------------------------------------------------------------
Sequence::Sequence(String name, std::vector<SequenceTrack>&& tracks)
	: Name(std::move(name)), Tracks(std::move(tracks))
{
}

//------------------------------------------------------------------------------
String Sequence::GetName() const
{
	return Name;
}

//------------------------------------------------------------------------------
bool Sequence::IsActive()
{
	bool result = false;

	for (auto& track : Tracks)
		if (track.IsActive())
		{
			result = true;
			break;
		}

	return result;
}

//------------------------------------------------------------------------------
void Sequence::OnBegin(Entity* entity)
{
	for (auto& track : Tracks)
		track.OnBegin(entity);
}

//------------------------------------------------------------------------------
void Sequence::OnUpdate(const TimeDuration deltaTime)
{
	for (auto& track : Tracks)
		track.OnUpdate(deltaTime);
}

//------------------------------------------------------------------------------
void Sequence::OnAbort()
{
	for (auto& track : Tracks)
		track.OnAbort();
}
