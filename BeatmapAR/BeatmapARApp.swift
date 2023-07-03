import SwiftUI
import RealityKit
import RealityKitContent
import BeatmapLoader

@main
struct BeatmapARApp: App {
    @Environment(\.dismissWindow) var dismissWindow

    var body: some SwiftUI.Scene {
        WindowGroup(id: "menu-window") {
            MenuScreen()
        }

        ImmersiveSpace(for: BeatmapSelection.self) { selection in
            if let selection = selection.wrappedValue {
                BeatmapSpace(selection: selection)
                    .onAppear(perform: { dismissWindow(id: "menu-window") })
            }
        }
        .immersionStyle(selection: .constant(.mixed), in: .mixed)
    }
}
