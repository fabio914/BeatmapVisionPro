import SwiftUI
import BeatmapLoader

struct SongScreen: View {
    @Binding var selection: BeatmapSelection?
    let filePreview: BeatmapFilePreview

    var body: some View {
        VStack(spacing: 20) {
            HStack {
                Image(uiImage: filePreview.preview.coverImage)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(width: 120, height: 120)
                VStack(alignment: .leading) {
                    Text("\(filePreview.preview.songAuthorName) - \(filePreview.preview.songName)")
                        .font(.title)
                    VStack(alignment: .leading) {
                        Text("Beats Per Minute")
                            .font(.headline)
                        Text("\(filePreview.preview.beatsPerMinute)")
                            .font(.caption)
                    }
                }
            }

            HStack {
                ForEach(filePreview.preview.standardDifficulties, id: \.rawValue) { difficulty in
                    Button(action: { selection = .init(url: filePreview.url, difficulty: difficulty) }, label: {
                        Text(difficulty.displayName)
                    })
                }
            }
        }
        .navigationTitle(filePreview.preview.songName)
    }
}
