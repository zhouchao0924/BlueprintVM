#include "AssetEditor/Architect/AssetArchitectToolkit.h"
#include "EditorStyleSet.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "GraphEditor.h"
#include "PropertyEditorModule.h"
#include "SAdvancedPreviewDetailsTab.h"
#include "Framework/Commands/GenericCommands.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "EdGraphUtilities.h"
#include "ScopedTransaction.h"
#include "SNodePanel.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Commands/BTCommands.h"
#include "Framework/Docking/TabManager.h"

#define LOCTEXT_NAMESPACE "BlueprintToolEditorToolkit"

struct FBPToolID
{
	static const FName ViewportID;
	static const FName PreviewID;
	static const FName ContentBrowserID;
	static const FName DetailsID;
	static const FName BPNodeListID;
	static const FName BlueprintGraphID;
	static const FName PreviewSettingsID;
};

const FName FBPToolID::PreviewID(TEXT("PreviewID"));
const FName FBPToolID::ViewportID(TEXT("ViewportID"));
const FName FBPToolID::ContentBrowserID(TEXT("ContentBrowserID"));
const FName FBPToolID::DetailsID(TEXT("DetailsID"));
const FName FBPToolID::BPNodeListID(TEXT("BPNodeListID"));
const FName FBPToolID::BlueprintGraphID(TEXT("BlueprintGraphID"));
const FName FBPToolID::PreviewSettingsID(TEXT("PreviewSettingsID"));

FBlueprintToolEditorToolkit::~FBlueprintToolEditorToolkit()
{

}

bool FBlueprintToolEditorToolkit::IsTickableInEditor() const
{
	return true;
}

void FBlueprintToolEditorToolkit::Tick(float DeltaTime)
{

}

bool FBlueprintToolEditorToolkit::IsTickable() const
{
	return true;
}

TStatId FBlueprintToolEditorToolkit::GetStatId() const
{
	return TStatId();
}

void FBlueprintToolEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	Super::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FBPToolID::PreviewID, FOnSpawnTab::CreateSP(this, &FBlueprintToolEditorToolkit::SpawnByPreviewTab))
		.SetDisplayName(LOCTEXT("PreviewTab", "BP_Preview"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(FBPToolID::ContentBrowserID, FOnSpawnTab::CreateSP(this, &FBlueprintToolEditorToolkit::SpawnByContentBrowserTab))
		.SetDisplayName(LOCTEXT("ContentBrowserLabel", "BP_Content Browser"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.ContentBrowser"));

	InTabManager->RegisterTabSpawner(FBPToolID::BlueprintGraphID, FOnSpawnTab::CreateSP(this, &FBlueprintToolEditorToolkit::SpawnByBlueprintViewTab))
		.SetDisplayName(LOCTEXT("BlueprintGraphTab", "BP_BlueprintGraph"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(FBPToolID::BPNodeListID, FOnSpawnTab::CreateSP(this, &FBlueprintToolEditorToolkit::SpawnByPaletteTab))
		.SetDisplayName(LOCTEXT("ActionsTabLabel", "MyNodeList"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FBPToolID::DetailsID, FOnSpawnTab::CreateSP(this, &FBlueprintToolEditorToolkit::SpawnByDetailsTab))
		.SetDisplayName(LOCTEXT("DetailsTabLabel", "MyDetails"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FBPToolID::PreviewSettingsID, FOnSpawnTab::CreateSP(this, &FBlueprintToolEditorToolkit::SpawnByPreviewSettingsTab))
		.SetDisplayName(LOCTEXT("PreviewSettingsTabLabel", "BP_Preview Settings"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FBlueprintToolEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	Super::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FBPToolID::PreviewID);
	InTabManager->UnregisterTabSpawner(FBPToolID::ContentBrowserID);
	InTabManager->UnregisterTabSpawner(FBPToolID::BlueprintGraphID);

	FBTCommands::Unregister();
}

void FBlueprintToolEditorToolkit::Initialize(
	UBlueprintData* InTextAsset,
	const EToolkitMode::Type InMode,
	const TSharedPtr<IToolkitHost>& InToolkitHost)
{
	//调用virtual void RegisterCommands() override;
	FBTCommands::Register();/** Use this method to register commands. Usually done in StartupModule(). */

	BindCommands();
	ExtendToolbar();

	const TSharedRef<FTabManager::FLayout> StandloneCustomLayout = FTabManager::NewLayout("StandloneBlueprintLayout_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.10f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.90f)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.20f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.33f)
						->SetHideTabWell(true)
						->AddTab(FBPToolID::PreviewID, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.66f)
						->SetHideTabWell(true)
						->AddTab(FBPToolID::DetailsID, ETabState::OpenedTab)
						->AddTab(FBPToolID::PreviewSettingsID, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.60f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.66f)
						->SetHideTabWell(true)
						->AddTab(FBPToolID::BlueprintGraphID, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.33f)
						->SetHideTabWell(true)
						->AddTab(FBPToolID::ContentBrowserID, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.20f)
					->SetHideTabWell(true)
					->AddTab(FBPToolID::BPNodeListID, ETabState::OpenedTab)
				)
			)
		);

	/**
	 * Initializes this asset editor.  Called immediately after construction.  If you override this, remember to
	 * call the base class implementation
	 *
	 * @param	Mode					Asset editing mode for this editor (standalone or world-centric)
	 * @param	InitToolkitHost			When Mode is WorldCentric, this is the level editor instance to spawn this editor within
	 * @param	AppIdentifier			When Mode is Standalone, this is the app identifier of the app that should host this toolkit
	 * @param	StandaloneDefaultLayout	The default layout for a standalone asset editor
	 * @param	bCreateDefaultToolbar	The default toolbar, which can be extended
	 * @param	bCreateDefaultStandaloneMenu	True if in standalone mode, the asset editor should automatically generate a default "asset" menu, or false if you're going to do this yourself in your derived asset editor's implementation
	 * @param	ObjectToEdit			The object to edit
	 * @param	bInIsToolbarFocusable	Whether the buttons on the default toolbar can receive keyboard focus
	 * @param	bUseSmallToolbarIcons	Whether the buttons on the default toolbar use the small icons
	 */
	 //void FAssetEditorToolkit::InitAssetEditor(
	 //	const EToolkitMode::Type Mode, 
	 //	const TSharedPtr< class IToolkitHost >& InitToolkitHost, 
	 //	const FName AppIdentifier, 
	 //	const TSharedRef<FTabManager::FLayout>& StandaloneDefaultLayout, 
	 //	const bool bCreateDefaultStandaloneMenu, 
	 //	const bool bCreateDefaultToolbar, 
	 //	UObject* ObjectToEdit, 
	 //	const bool bInIsToolbarFocusable, 
	 //	const bool bInUseSmallToolbarIcons)

	InitAssetEditor(
		InMode,
		InToolkitHost,
		FBPToolID::ViewportID,
		StandloneCustomLayout,
		true,
		true,
		InTextAsset
	);


	RegenerateMenusAndToolbars();
}

FName FBlueprintToolEditorToolkit::GetToolkitFName() const
{
	return FName("BlueprintToolEditorToolkit");
}

FText FBlueprintToolEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("BlueprintToolLabel", "BlueprintTool Asset Editor");
}

FString FBlueprintToolEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("BlueprintToolEditorPrefix", "BlueprintData").ToString();
}

FLinearColor FBlueprintToolEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

void FBlueprintToolEditorToolkit::BindCommands()
{
	const FBTCommands& Commands = FBTCommands::Get();

	ToolkitCommands->MapAction(
		Commands.Compile,
		FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::Compile));

	ToolkitCommands->MapAction(
		Commands.Help,
		FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::Help));

	ToolkitCommands->MapAction(
		Commands.Run,
		FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::Run));
}

void FBlueprintToolEditorToolkit::ExtendToolbar()
{
	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("Compile");
			{
				ToolbarBuilder.AddToolBarButton(FBTCommands::Get().Compile);
			}
			ToolbarBuilder.EndSection();

			ToolbarBuilder.BeginSection("Help");
			{
				ToolbarBuilder.AddToolBarButton(FBTCommands::Get().Help);
			}
			ToolbarBuilder.EndSection();

			ToolbarBuilder.BeginSection("Run");
			{
				ToolbarBuilder.AddToolBarButton(FBTCommands::Get().Run);
			}
			ToolbarBuilder.EndSection();
		}
	};

	TSharedPtr<FExtender> ToolbarExtender(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar));

	AddToolbarExtender(ToolbarExtender);
}

void FBlueprintToolEditorToolkit::Compile()
{

}

void FBlueprintToolEditorToolkit::Help()
{

}

void FBlueprintToolEditorToolkit::Run()
{

}

void FBlueprintToolEditorToolkit::SaveAsset_Execute()
{
	
}

UBlueprintData* FBlueprintToolEditorToolkit::GetBlueprintData() const
{
	return GetEditingObjects().Num() == 1 ? Cast<UBlueprintData>(GetEditingObjects()[0]) : NULL;
}

TSharedRef<SDockTab> FBlueprintToolEditorToolkit::SpawnByPreviewTab(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab);

	return SpawnedTab;
}

TSharedRef<SDockTab> FBlueprintToolEditorToolkit::SpawnByContentBrowserTab(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> SpawnedTab =
		SNew(SDockTab)
		.Label(LOCTEXT("BPContentBrowserKey", "Content Browser"))
		.TabColorScale(GetTabColorScale())
		[
			SNullWidget::NullWidget
		];

	IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
	FName ContentBrowserID = *("BP_ContentBrowser_" + FGuid::NewGuid().ToString());
	FContentBrowserConfig ContentBrowserConfig;

	TSharedRef<SWidget> ContentBrowser = ContentBrowserSingleton.CreateContentBrowser(ContentBrowserID, SpawnedTab, &ContentBrowserConfig);
	SpawnedTab->SetContent(ContentBrowser);

	return SpawnedTab;
}

TSharedRef<SDockTab> FBlueprintToolEditorToolkit::SpawnByBlueprintViewTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab);
}

TSharedRef<SDockTab> FBlueprintToolEditorToolkit::SpawnByPaletteTab(const FSpawnTabArgs& Args)
{

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab);

	return SpawnedTab;
}

TSharedRef<SDockTab> FBlueprintToolEditorToolkit::SpawnByDetailsTab(const FSpawnTabArgs& Args)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	const FDetailsViewArgs DetailsViewArgs(
		false,
		false,
		true,
		FDetailsViewArgs::HideNameArea,
		true,
		this);

	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	PropertyEditor = PropertyEditorRef;

	return SNew(SDockTab)
		.Label(LOCTEXT("BTDetailsTab", "Details"))
		[
			PropertyEditorRef
		];
}

TSharedRef<SDockTab> FBlueprintToolEditorToolkit::SpawnByPreviewSettingsTab(const FSpawnTabArgs& Args)
{


	return SNew(SDockTab);
}

TSharedRef<SGraphEditor> FBlueprintToolEditorToolkit::CreateBPGraphEditor(UEdGraph* InGraph)
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("MyAppearanceCornerText", "BP Editor");//右下角的字

	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("MyText", "Smartuil's Blueprint"))
		.TextStyle(FEditorStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
		]
		];

	//绑定对蓝图粘贴删除等操作
	GraphEditorCommands = MakeShareable(new FUICommandList);
	{
		GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
			FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::DuplicateNodes),
			FCanExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::CanDuplicateNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::CanSelectAllNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
			FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::CanDeleteNode)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
			FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::CopySelectedNodes),
			FCanExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::CanCopyNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
			FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::PasteNodes),
			FCanExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::CanPasteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
			FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::CutSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::CanCutNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Undo,
			FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::UndoGraphAction)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Redo,
			FExecuteAction::CreateSP(this, &FBlueprintToolEditorToolkit::RedoGraphAction)
		);
	}

	//处理点击不同node
	SGraphEditor::FGraphEditorEvents InGraphEvents;
	InGraphEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FBlueprintToolEditorToolkit::OnSelectedBPNodesChanged);

	TSharedRef<SGraphEditor> GraphEditorInstance = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.Appearance(AppearanceInfo)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InGraphEvents);

	return GraphEditorInstance;
}

void FBlueprintToolEditorToolkit::OnSelectedBPNodesChanged(const TSet<class UObject*>& SelectionNode)
{

}

void FBlueprintToolEditorToolkit::AssetDropped(UObject* AssetObject)
{

}

bool FBlueprintToolEditorToolkit::IsAssetAcceptableForDrop(const UObject* AssetObject) const
{
	return true;
}

FVector2D FBlueprintToolEditorToolkit::GetAssetDropGridLocation() const
{
	FVector2D GridLocation;

	return GridLocation;
}

void FBlueprintToolEditorToolkit::OnGraphChanged(const FEdGraphEditAction& Action)
{
	bGraphChanged = true;
}

void FBlueprintToolEditorToolkit::GraphChanged()
{

}

void FBlueprintToolEditorToolkit::DeleteSelectedNodes()
{
	
}

bool FBlueprintToolEditorToolkit::CanDeleteNode() const
{
	return true;
}

void FBlueprintToolEditorToolkit::CopySelectedNodes()
{

}

bool FBlueprintToolEditorToolkit::CanCopyNodes() const
{
	return true;
}

void FBlueprintToolEditorToolkit::PasteNodes()
{
	
}

bool FBlueprintToolEditorToolkit::CanPasteNodes() const
{
	return true;
}

void FBlueprintToolEditorToolkit::CutSelectedNodes()
{
	
}

bool FBlueprintToolEditorToolkit::CanCutNodes() const
{
	return true;
}

void FBlueprintToolEditorToolkit::DuplicateNodes()
{
	
}

bool FBlueprintToolEditorToolkit::CanDuplicateNodes() const
{
	return true;
}

void FBlueprintToolEditorToolkit::SelectAllNodes()
{
	
}

bool FBlueprintToolEditorToolkit::CanSelectAllNodes() const
{
	return true;
}

void FBlueprintToolEditorToolkit::UndoGraphAction()
{

}

void FBlueprintToolEditorToolkit::RedoGraphAction()
{
	
}

#undef LOCTEXT_NAMESPACE