//
//  OggDecoder.h
//  OggDecoder
//
//  Created by Fabio Dela Antonio on 04/02/2024.
//

#ifndef OGGDecoder_h
#define OGGDecoder_h

#import <Foundation/Foundation.h>

//! Project version number for OggDecoder.
FOUNDATION_EXPORT double OggDecoderVersionNumber;

//! Project version string for OggDecoder.
FOUNDATION_EXPORT const unsigned char OggDecoderVersionString[];

NS_ASSUME_NONNULL_BEGIN

@interface OGGDecoder : NSObject
-(BOOL)decode:(NSURL*)oggFile into:(NSURL*)outputFile;
-(void)decode:(NSURL*)oggFile into:(NSURL*)outputFile completion:(void (^)(BOOL))completion;
-(nullable NSURL*)decode:(NSURL*)oggFile;
-(void)decode:(NSURL*)oggFile completion:(void (^)(NSURL* _Nullable outputFile))completion;
@end

NS_ASSUME_NONNULL_END

#endif /* OGGDecoder_h */
