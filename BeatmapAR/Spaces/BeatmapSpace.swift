import SwiftUI
import RealityKit

struct BeatmapSpace: View {
    let selection: BeatmapSelection

    init(selection: BeatmapSelection) {
        self.selection = selection
    }

    var body: some View {
        RealityView(make: { content in
            if let sceneManager = try? await SceneManager(url: selection.url, difficulty: selection.difficulty) {
                content.add(sceneManager.anchor)

                Task {
                    var elapsedTime = 0.0
                    for await event in CADisplayLink.events() {
                        elapsedTime += (event.targetTimestamp - event.timestamp)
                        sceneManager.update(elapsedTime: elapsedTime)
                    }
                }
            }
        }, update: { updateContent in

        })
    }
}
