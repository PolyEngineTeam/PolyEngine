#include <Defines.hpp>
#include <Math/BasicMath.hpp>
#include <catch.hpp>

#include <ECS/Entity.hpp>
#include <Sequences/Data/IAction.hpp>
#include <Sequences/Data/SequenceTrack.hpp>

class TestAction : public Poly::IAction
{
public:
	TestAction(TimeDuration duration)
		: Duration(duration)
	{

	}

	TimeDuration GetDuration() const
	{
		return Duration;
	}

	void OnBegin(Poly::Entity* entity) final
	{
		EntityObjects.push_back(entity);
		++OnBeginCalls;
	}

	void OnUpdate(TimeDuration dt) final
	{
		DeltaTimes.push_back(dt);
		TimeElapsed += dt;
		++OnUpdateCalls;
	}

	void OnAbort() final
	{
		++OnAbortCalls;
	}

	void OnFinish() final
	{
		++OnFinishCalls;
	}

	const std::vector<Poly::Entity*>& GetEntityObjects() const { return EntityObjects; }
	const std::vector<TimeDuration>& GetDeltaTimes() const { return DeltaTimes; }
	TimeDuration GetTimeElapsed() const { return TimeElapsed; }
	int GetOnBeginCalls() const { return OnBeginCalls; }
	int GetOnUpdateCalls() const { return OnUpdateCalls; }
	int GetOnAbortCalls() const { return OnAbortCalls; }
	int GetOnFinishCalls() const { return OnFinishCalls; }

private:
	const TimeDuration Duration = TimeDuration(0);

	std::vector<Poly::Entity*> EntityObjects;
	std::vector<TimeDuration> DeltaTimes;
	TimeDuration TimeElapsed = TimeDuration(0);
	int OnBeginCalls = 0;
	int OnUpdateCalls = 0;
	int OnAbortCalls = 0;
	int OnFinishCalls = 0;
};

TEST_CASE("Single action.", "[SequenceTrack]")
{
	Poly::SequenceTrack track;

	auto action = std::make_shared<TestAction>(TimeDuration(1));

	SECTION("Start and finish action with single udate immidiately")
	{
		track.AppendAction(TimeDuration(0), action);
		CHECK(track.IsActive() == false);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
		CHECK(action->GetOnBeginCalls() == 0);
		CHECK(action->GetOnUpdateCalls() == 0);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 0);

		track.OnBegin(new Poly::Entity());
		CHECK(track.IsActive() == true);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
		CHECK(action->GetOnBeginCalls() == 0);
		CHECK(action->GetOnUpdateCalls() == 0);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 0);

		track.OnUpdate(TimeDuration(action->GetDuration()));
		CHECK(track.IsActive() == false);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(1.).count()));
		CHECK(action->GetOnBeginCalls() == 1);
		CHECK(action->GetOnUpdateCalls() == 1);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 1);
	}

	SECTION("Start and finish action with two updates immidiately")
	{
		track.AppendAction(TimeDuration(0), action);
		CHECK(track.IsActive() == false);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
		CHECK(action->GetOnBeginCalls() == 0);
		CHECK(action->GetOnUpdateCalls() == 0);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 0);

		track.OnBegin(new Poly::Entity());
		CHECK(track.IsActive() == true);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
		CHECK(action->GetOnBeginCalls() == 0);
		CHECK(action->GetOnUpdateCalls() == 0);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 0);

		track.OnUpdate(TimeDuration(0.5));
		CHECK(track.IsActive() == true);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.5).count()));
		CHECK(action->GetOnBeginCalls() == 1);
		CHECK(action->GetOnUpdateCalls() == 1);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 0);

		track.OnUpdate(TimeDuration(0.5));
		CHECK(track.IsActive() == false);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(1.).count()));
		CHECK(action->GetOnBeginCalls() == 1);
		CHECK(action->GetOnUpdateCalls() == 2);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 1);
	}

	SECTION("Start action with delay")
	{
		track.AppendAction(TimeDuration(1), action);
		CHECK(track.IsActive() == false);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
		CHECK(action->GetOnBeginCalls() == 0);
		CHECK(action->GetOnUpdateCalls() == 0);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 0);

		track.OnBegin(new Poly::Entity());
		CHECK(track.IsActive() == true);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
		CHECK(action->GetOnBeginCalls() == 0);
		CHECK(action->GetOnUpdateCalls() == 0);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 0);

		track.OnUpdate(TimeDuration(1.0));
		CHECK(track.IsActive() == true);
		CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(1.0).count()));
		CHECK(action->GetOnBeginCalls() == 1);
		CHECK(action->GetOnUpdateCalls() == 1);
		CHECK(action->GetOnAbortCalls() == 0);
		CHECK(action->GetOnFinishCalls() == 0);
	}

	SECTION("Start immidiately.")
	{
		SECTION("OnFinish")
		{
			track.AppendAction(TimeDuration(0), action);
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnBegin(new Poly::Entity());
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.1));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 1);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 2);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(1.0).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 3);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 1);
		}

		SECTION("OnAbort")
		{
			track.AppendAction(TimeDuration(0), action);
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnBegin(new Poly::Entity());
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.1));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 1);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 2);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnAbort();
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 2);
			CHECK(action->GetOnAbortCalls() == 1);
			CHECK(action->GetOnFinishCalls() == 0);
		}
	}
	
	SECTION("Start with delay.")
	{
		SECTION("OnFinish")
		{
			track.AppendAction(TimeDuration(1), action);
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnBegin(new Poly::Entity());
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.1));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.0).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.0).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 1);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 2);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(5));
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(1.).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 3);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 1);
		}

		SECTION("OnAbort")
		{
			track.AppendAction(TimeDuration(1), action);
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnBegin(new Poly::Entity());
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.1));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.0).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.0).count()));
			CHECK(action->GetOnBeginCalls() == 0);
			CHECK(action->GetOnUpdateCalls() == 0);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 1);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 2);
			CHECK(action->GetOnAbortCalls() == 0);
			CHECK(action->GetOnFinishCalls() == 0);

			track.OnAbort();
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)action->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(action->GetOnBeginCalls() == 1);
			CHECK(action->GetOnUpdateCalls() == 2);
			CHECK(action->GetOnAbortCalls() == 1);
			CHECK(action->GetOnFinishCalls() == 0);
		}
	}
}

TEST_CASE("Two actions.", "[SequenceTrack]")
{
	Poly::SequenceTrack track;

	auto firstAction = std::make_shared<TestAction>(TimeDuration(2));
	auto secondAction = std::make_shared<TestAction>(TimeDuration(1));

	// compilation helper
	auto action = std::make_shared<TestAction>(TimeDuration(1));

	SECTION("Start second action immidiately")
	{
		SECTION("OnFinish")
			{
				track.AppendAction(TimeDuration(0), firstAction);
				track.AppendAction(TimeDuration(2), secondAction);
				CHECK(track.IsActive() == false);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
				CHECK(firstAction->GetOnBeginCalls() == 0);
				CHECK(firstAction->GetOnUpdateCalls() == 0);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 0);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
				CHECK(secondAction->GetOnBeginCalls() == 0);
				CHECK(secondAction->GetOnUpdateCalls() == 0);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 0);

				track.OnBegin(new Poly::Entity());
				CHECK(track.IsActive() == true);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
				CHECK(firstAction->GetOnBeginCalls() == 0);
				CHECK(firstAction->GetOnUpdateCalls() == 0);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 0);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
				CHECK(secondAction->GetOnBeginCalls() == 0);
				CHECK(secondAction->GetOnUpdateCalls() == 0);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 0);

				track.OnUpdate(TimeDuration(0.1));
				CHECK(track.IsActive() == true);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
				CHECK(firstAction->GetOnBeginCalls() == 1);
				CHECK(firstAction->GetOnUpdateCalls() == 1);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 0);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
				CHECK(secondAction->GetOnBeginCalls() == 0);
				CHECK(secondAction->GetOnUpdateCalls() == 0);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 0);

				track.OnUpdate(TimeDuration(0.5));
				CHECK(track.IsActive() == true);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
				CHECK(firstAction->GetOnBeginCalls() == 1);
				CHECK(firstAction->GetOnUpdateCalls() == 2);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 0);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
				CHECK(secondAction->GetOnBeginCalls() == 0);
				CHECK(secondAction->GetOnUpdateCalls() == 0);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 0);

				track.OnUpdate(TimeDuration(0.5));
				CHECK(track.IsActive() == true);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(1.1).count()));
				CHECK(firstAction->GetOnBeginCalls() == 1);
				CHECK(firstAction->GetOnUpdateCalls() == 3);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 0);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
				CHECK(secondAction->GetOnBeginCalls() == 0);
				CHECK(secondAction->GetOnUpdateCalls() == 0);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 0);

				track.OnUpdate(TimeDuration(0.5));
				CHECK(track.IsActive() == true);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(1.6).count()));
				CHECK(firstAction->GetOnBeginCalls() == 1);
				CHECK(firstAction->GetOnUpdateCalls() == 4);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 0);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
				CHECK(secondAction->GetOnBeginCalls() == 0);
				CHECK(secondAction->GetOnUpdateCalls() == 0);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 0);

				track.OnUpdate(TimeDuration(0.5));
				CHECK(track.IsActive() == true);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(2.).count()));
				CHECK(firstAction->GetOnBeginCalls() == 1);
				CHECK(firstAction->GetOnUpdateCalls() == 5);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 1);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
				CHECK(secondAction->GetOnBeginCalls() == 1);
				CHECK(secondAction->GetOnUpdateCalls() == 1);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 0);

				track.OnUpdate(TimeDuration(0.5));
				CHECK(track.IsActive() == true);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(2.).count()));
				CHECK(firstAction->GetOnBeginCalls() == 1);
				CHECK(firstAction->GetOnUpdateCalls() == 5);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 1);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
				CHECK(secondAction->GetOnBeginCalls() == 1);
				CHECK(secondAction->GetOnUpdateCalls() == 2);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 0);

				track.OnUpdate(TimeDuration(0.5));
				CHECK(track.IsActive() == false);
				CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(2.).count()));
				CHECK(firstAction->GetOnBeginCalls() == 1);
				CHECK(firstAction->GetOnUpdateCalls() == 5);
				CHECK(firstAction->GetOnAbortCalls() == 0);
				CHECK(firstAction->GetOnFinishCalls() == 1);
				CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(1.).count()));
				CHECK(secondAction->GetOnBeginCalls() == 1);
				CHECK(secondAction->GetOnUpdateCalls() == 3);
				CHECK(secondAction->GetOnAbortCalls() == 0);
				CHECK(secondAction->GetOnFinishCalls() == 1);
			}

		SECTION("OnAbort first action")
		{
			track.AppendAction(TimeDuration(0), firstAction);
			track.AppendAction(TimeDuration(2), secondAction);
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 0);
			CHECK(firstAction->GetOnUpdateCalls() == 0);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnBegin(new Poly::Entity());
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 0);
			CHECK(firstAction->GetOnUpdateCalls() == 0);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.1));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 1);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 2);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(1.1).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 3);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(1.6).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 4);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnAbort();
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(1.6).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 4);
			CHECK(firstAction->GetOnAbortCalls() == 1);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.0).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);
		}

		SECTION("OnAbort second action")
		{
			track.AppendAction(TimeDuration(0), firstAction);
			track.AppendAction(TimeDuration(2), secondAction);
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 0);
			CHECK(firstAction->GetOnUpdateCalls() == 0);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnBegin(new Poly::Entity());
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 0);
			CHECK(firstAction->GetOnUpdateCalls() == 0);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.1));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 1);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 2);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(1.1).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 3);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(1.6).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 4);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(2.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 5);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 1);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(secondAction->GetOnBeginCalls() == 1);
			CHECK(secondAction->GetOnUpdateCalls() == 1);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.5));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(2.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 5);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 1);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(secondAction->GetOnBeginCalls() == 1);
			CHECK(secondAction->GetOnUpdateCalls() == 2);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnAbort();
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(2.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 5);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 1);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.6).count()));
			CHECK(secondAction->GetOnBeginCalls() == 1);
			CHECK(secondAction->GetOnUpdateCalls() == 2);
			CHECK(secondAction->GetOnAbortCalls() == 1);
			CHECK(secondAction->GetOnFinishCalls() == 0);
		}

		SECTION("OnAbort")
		{
			track.AppendAction(TimeDuration(0), firstAction);
			track.AppendAction(TimeDuration(0.2), secondAction);
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 0);
			CHECK(firstAction->GetOnUpdateCalls() == 0);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnBegin(new Poly::Entity());
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 0);
			CHECK(firstAction->GetOnUpdateCalls() == 0);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.1));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 1);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnAbort();
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 1);
			CHECK(firstAction->GetOnAbortCalls() == 1);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.0).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			CHECK(track.IsActive() == false);
		}
		SECTION("Assert hit!")
		{
			track.AppendAction(TimeDuration(0), firstAction);
			track.AppendAction(TimeDuration(0.2), secondAction);
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 0);
			CHECK(firstAction->GetOnUpdateCalls() == 0);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnBegin(new Poly::Entity());
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(firstAction->GetOnBeginCalls() == 0);
			CHECK(firstAction->GetOnUpdateCalls() == 0);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnUpdate(TimeDuration(0.3));
			CHECK(track.IsActive() == true);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 1);
			CHECK(firstAction->GetOnAbortCalls() == 0);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			track.OnAbort();
			CHECK(track.IsActive() == false);
			CHECK(Poly::CmpfFast((float)firstAction->GetTimeElapsed().count(), (float)TimeDuration(0.1).count()));
			CHECK(firstAction->GetOnBeginCalls() == 1);
			CHECK(firstAction->GetOnUpdateCalls() == 1);
			CHECK(firstAction->GetOnAbortCalls() == 1);
			CHECK(firstAction->GetOnFinishCalls() == 0);
			CHECK(Poly::CmpfFast((float)secondAction->GetTimeElapsed().count(), (float)TimeDuration(0.0).count()));
			CHECK(secondAction->GetOnBeginCalls() == 0);
			CHECK(secondAction->GetOnUpdateCalls() == 0);
			CHECK(secondAction->GetOnAbortCalls() == 0);
			CHECK(secondAction->GetOnFinishCalls() == 0);

			CHECK(track.IsActive() == false);
		}
	}
}