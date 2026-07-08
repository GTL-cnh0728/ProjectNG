// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "UI/WidgetController/NGMapWidgetController.h"

#include "Game/NGGameState.h"
#include "Kismet/GameplayStatics.h"

void UNGMapWidgetController::BroadcastInitialValues()
{
	// 이미 GameState에 맵 데이터가 복제되어 있다면 즉시 브로드캐스트
	// (늦게 접속한 유저 대응)
	BroadcastMapData();
}

void UNGMapWidgetController::BindCallbacksToDependencies()
{
	if (!PlayerController) return;

	if (ANGGameState* GS = Cast<ANGGameState>(UGameplayStatics::GetGameState(PlayerController)))
	{
		GS->OnMapDataReady.AddDynamic(this, &UNGMapWidgetController::OnMapDataReady);
	}
}

void UNGMapWidgetController::HandleNodeSelected(int32 NodeID)
{
	UE_LOG(LogTemp, Log, TEXT("[MapWidgetController] Node Selected: %d"), NodeID);

	// TODO: 서버에 이동 요청 RPC를 보내는 로직 확장 포인트
	// 예: PlayerController->Server_RequestMoveToNode(NodeID);
}

void UNGMapWidgetController::BroadcastMapData()
{
	if (!PlayerController) return;

	if (ANGGameState* GS = Cast<ANGGameState>(UGameplayStatics::GetGameState(PlayerController)))
	{
		if (GS->MapNodes.Num() > 0)
		{
			OnMapNodesReceived.Broadcast(GS->MapNodes);
		}
	}
}

void UNGMapWidgetController::OnMapDataReady()
{
	BroadcastMapData();
}
