// Includes
//=========

#include "cMyGame.h"
#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		//Fast Mode
		SetSimulationRate(1.5f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		//Slow Mode
		SetSimulationRate(0.5f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{
		//Pause Game
		SetSimulationRate(0.0f);
	}
	else
	{
		//Regular Mode
		SetSimulationRate(1.0f);
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	Logging::OutputMessage("MyGame has been successfully initialized!");
	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	Logging::OutputMessage("MyGame has been successfully cleaned up!");
	return Results::Success;
}
