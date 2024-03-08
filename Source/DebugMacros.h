#pragma once

#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.0f, 12, FColor::Red, true);
#define DRAW_SPHERE(Location, Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.0f, 12, Color, true);
#define DRAW_LINE(Start, End) if (GetWorld()) DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);
#define DRAW_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.0f, FColor::Red, true);
#define DRAW_BOX(Location) if(GetWorld()) DrawDebugBox(GetWorld(), Location, FVector(50.0f), FColor::Red, false);
#define DRAW_VECTOR(Start, End) if (GetWorld()) \
{\
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false);\
	DrawDebugPoint(GetWorld(), End, 15.0f, FColor::Red, false);\
}