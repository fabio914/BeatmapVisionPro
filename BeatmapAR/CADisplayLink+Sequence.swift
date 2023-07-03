import UIKit

@MainActor
extension CADisplayLink {
    static func events() -> AsyncStream<CADisplayLink> {
        AsyncStream { continuation in
            let displayLink = DisplayLink { displayLink in
                continuation.yield(displayLink)
            }

            continuation.onTermination = { _ in
                Task { await displayLink.stop() }
            }
        }
    }
}

private extension CADisplayLink {
    @MainActor
    private class DisplayLink: NSObject {
        private var displayLink: CADisplayLink!
        private let handler: (CADisplayLink) -> Void

        init(mode: RunLoop.Mode = .default, handler: @escaping (CADisplayLink) -> Void) {
            self.handler = handler
            super.init()

            displayLink = CADisplayLink(target: self, selector: #selector(handle(displayLink:)))
            displayLink.add(to: .main, forMode: mode)
        }

        func stop() {
            displayLink.invalidate()
        }

        @objc func handle(displayLink: CADisplayLink) {
            handler(displayLink)
        }
    }
}
